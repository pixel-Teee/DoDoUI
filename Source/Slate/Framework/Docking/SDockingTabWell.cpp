#include <PreCompileHeader.h>

#include "SDockingTabWell.h"

#include "FDockingDragOperation.h" //FDockingDragOperation depends on it

#include "SDockingTabStack.h"

#include "SlateCore/Layout/ArrangedChildren.h"

#include "SlateCore/Types/PaintArgs.h"

#include "SlateCore/Rendering/DrawElements.h"

namespace DoDo {
	const glm::vec2 FDockingConstants::max_minor_tab_size(160.0f, 50.0f);//todo:fix me, this will affect the calculate of SDock tab size
	const glm::vec2 FDockingConstants::max_major_tab_size(210.0f, 50.0f);

	const glm::vec2 FDockingConstants::get_max_tab_size_for(ETabRole tab_role)
	{
		return (tab_role == ETabRole::MajorTab)
			? max_major_tab_size
			: max_minor_tab_size;
	}

	SDockingTabWell::SDockingTabWell()
		: m_tabs(this)
	{
	}
	void SDockingTabWell::Construct(const FArguments& in_args)
	{
		m_foreground_tab_index = -1;
		m_tab_being_dragged_ptr = nullptr;
		m_child_being_dragged_offset = 0.0f;
		m_tab_grab_offset_fraction = glm::vec2(0.0f);

		m_separator_brush = nullptr;//no separator between tabs

		//we need a valid parent here, tab panels must exist in a SDockingNode
		m_parent_tab_stack_ptr = in_args._ParentStackNode.Get();
	}

	std::shared_ptr<SDockTab> SDockingTabWell::get_foreground_tab() const
	{
		if (m_tab_being_dragged_ptr)
		{
			return m_tab_being_dragged_ptr;
		}

		//get the active tab
		return (m_tabs.num() > 0 && m_foreground_tab_index > -1) ? m_tabs[m_foreground_tab_index] : std::shared_ptr<SDockTab>();
	}

	void SDockingTabWell::refresh_parent_cotent()
	{
		if (m_tabs.num() > 0 && m_foreground_tab_index != -1)
		{
			const std::shared_ptr<SDockTab>& foreground_tab = m_tabs[m_foreground_tab_index];

			//todo:call global tab manager set active tab

			std::shared_ptr<SWindow> parent_window_ptr = foreground_tab->get_parent_window();

			if (parent_window_ptr) //todo:add global tab manager get root window check
			{
				//todo:set title
			}

			std::shared_ptr<SDockingTabStack> parent_tab_stack = m_parent_tab_stack_ptr.lock();

			FDockingStackOptionalContent optional_content{};
			optional_content.m_content_left = foreground_tab->get_left_content();
			optional_content.m_content_right = foreground_tab->get_right_content();
			
			//note:this function is important
			parent_tab_stack->set_node_content(foreground_tab->get_content(), optional_content);
		}
	}

	std::shared_ptr<SDockingArea> SDockingTabWell::get_dock_area()
	{
		return m_parent_tab_stack_ptr.lock() ? m_parent_tab_stack_ptr.lock()->get_dock_area() : std::shared_ptr<SDockingArea>();
	}

	void SDockingTabWell::add_tab(const std::shared_ptr<SDockTab>& in_tab, int32_t at_index, bool b_keep_inactive)
	{
		in_tab->set_parent(std::static_pointer_cast<SDockingTabWell>(shared_from_this()));

		//add the tab and implicitly activate it
		if (at_index == -1)
		{
			this->m_tabs.add(in_tab);
			if (!b_keep_inactive)
			{
				bring_tab_to_front(m_tabs.num() - 1);
			}
		}
		else
		{
			at_index = std::clamp(at_index, 0, m_tabs.num());
			this->m_tabs.insert(in_tab, at_index);

			if (!b_keep_inactive)
			{
				bring_tab_to_front(at_index);
			}
		}

		//todo:implement this
	}

	void SDockingTabWell::bring_tab_to_front(int32_t tab_index_to_active)
	{
		const bool b_active_index_changing = tab_index_to_active != m_foreground_tab_index;

		if (b_active_index_changing)
		{
			const int32_t last_foreground_tab_index = std::min(m_foreground_tab_index, m_tabs.num() - 1);

			//for positive indexes, don't go out of bounds on the array
			m_foreground_tab_index = std::min(tab_index_to_active, m_tabs.num() - 1);

			std::shared_ptr<SDockingArea> my_dock_area = get_dock_area();

			if (m_tabs.num() > 0 && my_dock_area)
			{
				//todo:implement this
				const std::shared_ptr<SDockTab> previous_foreground_tab = (last_foreground_tab_index == -1) ?
					std::shared_ptr<SDockTab>()
					: m_tabs[last_foreground_tab_index];

				const std::shared_ptr<SDockTab> new_foreground_tab = (m_foreground_tab_index == -1) ?
					std::shared_ptr<SDockTab>()
					: m_tabs[m_foreground_tab_index];

			}
		}

		//always force a refresh, even if we don't think the active index changed
		//note:this function will set the content of this tab
		refresh_parent_cotent();
	}

	void SDockingTabWell::bring_tab_to_front(std::shared_ptr<SDockTab> tab_to_activate)
	{
		if (m_tabs.num() > 0)
		{
			for (int32_t tab_index = 0; tab_index < m_tabs.num(); ++tab_index)
			{
				if (m_tabs[tab_index] == tab_to_activate)
				{
					bring_tab_to_front(tab_index);
					return;
				}
			}
		}
	}

	glm::vec2 SDockingTabWell::Compute_Child_Size(const FGeometry& allotted_geometry) const
	{
		const int32_t num_children = m_tabs.num();

		/*assume all tabs overlap the same amount*/
		const float overlap_width = (num_children > 0)
			? m_tabs[0]->get_overlap_width()
			: 0.0f;

		//all children shall be the same size, evenly divide the allocated area
		//if we are dragging a tab, don't forget to take it into account when dividing
		const glm::vec2 child_size = m_tab_being_dragged_ptr ?
			glm::vec2((allotted_geometry.get_local_size().x - overlap_width) / (num_children + 1) + overlap_width, allotted_geometry.get_local_size().y)
			: glm::vec2((allotted_geometry.get_local_size().x - overlap_width) / num_children + overlap_width, allotted_geometry.get_local_size().y);
		
		//major vs .minor tabs have different tabs sizes
		std::shared_ptr<SDockTab> first_tab = (num_children > 0) ? m_tabs[0] : m_tab_being_dragged_ptr;

		//if there are no tabs in this tabwell, assume minor tabs
		glm::vec2 max_tab_size(0.0f, 0.0f);
		if (first_tab)
		{
			const ETabRole role_to_use = first_tab->get_visual_tab_role();
			max_tab_size = FDockingConstants::get_max_tab_size_for(role_to_use);
		}
		else
		{
			max_tab_size = FDockingConstants::max_minor_tab_size;
		}

		//don't let the tabs get too big, or they'll look ugly
		return glm::vec2(
			glm::min(child_size.x, max_tab_size.x),
			glm::min(child_size.y, max_tab_size.y)
		);
	}

	void SDockingTabWell::On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const
	{
		//the specialized tab well is dedicated to arranging tabs
		//tabs have uniform sizing (all tabs the same size)
		//tabwell also ignores widget visibility, as it is not really
		//relevant

		//the tab that is being dragged by the user, if any
		std::shared_ptr<SDockTab> tab_being_dragged = m_tab_being_dragged_ptr;

		const int32_t num_children = m_tabs.num();

		//tabs have a uniform size
		const glm::vec2 child_size = Compute_Child_Size(allotted_geometry);

		const float dragged_child_center = m_child_being_dragged_offset + child_size.x / 2.0f;//todo:add child being dragged offset

		//arrange all the tabs left to right
		float x_offset = 0.0f;
		for (int32_t tab_index = 0; tab_index < num_children; ++tab_index)
		{
			const std::shared_ptr<SDockTab> cur_tab = m_tabs[tab_index];

			const float child_width_with_overlap = child_size.x - cur_tab->get_overlap_width();

			//this tab being dragged is arranged later, it should not be arranged twice
			if (cur_tab == tab_being_dragged)
			{
				continue;
			}

			//is this spot reserved from the tab that is being dragged?
			if (tab_being_dragged && x_offset <= dragged_child_center && dragged_child_center < (x_offset + child_width_with_overlap))
			{
				//if so, leave some room to signify that this is where the dragged tab would end up
				x_offset += child_width_with_overlap;
			}

			arranged_children.add_widget(allotted_geometry.make_child(cur_tab, glm::vec2(x_offset, 0.0f), child_size));

			x_offset += child_width_with_overlap;
		}

		//arrange the tab currently being dragged by the user, if any
		
		//todo:implement child being dragged offset
		if (tab_being_dragged)
		{
			arranged_children.add_widget(allotted_geometry.make_child(tab_being_dragged, glm::vec2(m_child_being_dragged_offset, 0.0f), child_size));
		}
	}

	int32_t SDockingTabWell::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		//when we are dragging a tab, it must be painted on top of the other tabs, so we cannot
		//just reuse the panel's default on paint

		//the tabwell has no visualization of its own, it just visualizes its child tabs
		FArrangedChildren arranged_children(EVisibility::visible);
		this->Arrange_Children(allotted_geometry, arranged_children);

		//because we paint multiple children, we must track the maximum layer id that they produced in case one of our parents
		//wants to an overlay for all of its contents
		int32_t max_layer_id = layer_id;

		std::shared_ptr<SDockTab> foreground_tab = get_foreground_tab();

		FArrangedWidget* foreground_tab_geometry = nullptr;

		//draw all inactive tabs first, from last, to first, so that the inactive tabs
		//that come later, are drawn behind tabs that come before it
		for (int32_t child_index = arranged_children.num() - 1; child_index >= 0; --child_index)
		{
			FArrangedWidget& cur_widget = arranged_children[child_index];
			if (cur_widget.m_widget == foreground_tab)
			{
				foreground_tab_geometry = &cur_widget;
			}
			else
			{
				bool b_should_draw_separator = false;
				if (m_separator_brush && m_separator_brush->m_draw_as != ESlateBrushDrawType::NoDrawType
					&& (child_index + 1) >= 0 && (child_index + 1) < arranged_children.num()) //todo:implement is valid index
				{
					const FArrangedWidget& prev_widget = arranged_children[child_index + 1];
					b_should_draw_separator = !cur_widget.m_widget->is_hovered() && !prev_widget.m_widget->is_hovered() &&
						prev_widget.m_widget != foreground_tab;
				}

				//note:how to understand it?
				FSlateRect child_clip_rect = my_culling_rect.intersection_with(cur_widget.m_geometry.get_layout_bounding_rect());
				const int32_t cur_widgets_max_layer_id = cur_widget.m_widget->paint(args.with_new_parent(this), cur_widget.m_geometry, child_clip_rect, out_draw_elements,
					max_layer_id, in_widget_style, should_be_enabled(b_parent_enabled));

				if (b_should_draw_separator)
				{
					const float separator_height = cur_widget.m_geometry.get_local_size().y * 0.65f;

					//center the separator
					float offset = (cur_widget.m_geometry.get_local_size().y - separator_height) / 2.0f;
					FPaintGeometry geometry = cur_widget.m_geometry.to_paint_geometry(glm::vec2(cur_widget.m_geometry.get_local_size().x + 1.0f, offset),
						glm::vec2(1.0f, separator_height));

					//this code rounds the position of the widget so we don't end up on half a pixel and end up with
					//a larger size separator than we want
					FSlateRenderTransform new_transform = geometry.get_accumulated_render_transform();
					new_transform.set_translation(new_transform.get_translation());//todo:implement round to vector
					geometry.set_render_transform(new_transform);

					FSlateDrawElement::MakeBox(out_draw_elements,
						max_layer_id,
						geometry,
						m_separator_brush,
						ESlateDrawEffect::None,
						m_separator_brush->get_tint(in_widget_style));//todo:fix me
				}

				max_layer_id = std::max(max_layer_id, cur_widgets_max_layer_id);
			}
		}

		//draw active tab in front
		if (foreground_tab != std::shared_ptr<SDockTab>())
		{
			FSlateRect child_clip_rect = my_culling_rect.intersection_with(foreground_tab_geometry->m_geometry.get_layout_bounding_rect());
			const int32_t cur_widgets_max_layer_id = foreground_tab_geometry->m_widget->paint(args.with_new_parent(this),
				foreground_tab_geometry->m_geometry, child_clip_rect, out_draw_elements,
				max_layer_id, in_widget_style, should_be_enabled(b_parent_enabled));

			max_layer_id = std::max(max_layer_id, cur_widgets_max_layer_id);
		}

		return max_layer_id;
	}

	void SDockingTabWell::On_Drag_Enter(const FGeometry& my_geometry, const FDragDropEvent& drag_drop_event)
	{
		std::shared_ptr<FDockingDragOperation> drag_drop_operation = drag_drop_event.get_operation_as<FDockingDragOperation>();

		if(drag_drop_operation)
		{
			//note:first to get the parent tab stack from SDockingTabWell, then to see drag drop event's tab can dock in this stack
			if(drag_drop_operation->can_dock_in_node(m_parent_tab_stack_ptr.lock(), FDockingDragOperation::DockingViaTabWell))
			{
				//the user dragged a tab into this tab well

				//update the state of the DragDropOperation to reflect this change
				drag_drop_operation->on_tab_well_entered(std::static_pointer_cast<SDockingTabWell>(shared_from_this()));

				//preview the position of the tab in the TabWell
				//from drag drop operation to get this tab
				this->m_tab_being_dragged_ptr = drag_drop_operation->get_tab_being_dragged();

				//add the tab widget to the well when the tab is dragged in
				m_tabs.add(m_tab_being_dragged_ptr);

				//todo:implement tab grab offset fraction
				this->m_tab_grab_offset_fraction = drag_drop_operation->get_tab_grab_offset_fraction();

				//the user should see the contents of the tab that we're dragging
				m_parent_tab_stack_ptr.lock()->set_node_content(drag_drop_operation->get_tab_being_dragged()->get_content(), FDockingStackOptionalContent());
			}
		}
	}
	FReply SDockingTabWell::On_Drag_Over(const FGeometry& my_geometry, const FDragDropEvent& drag_drop_event)
	{
		std::shared_ptr<FDockingDragOperation> drag_drop_operation = drag_drop_event.get_operation_as<FDockingDragOperation>();

		if (drag_drop_operation)
		{
			if (drag_drop_operation->can_dock_in_node(m_parent_tab_stack_ptr.lock(), FDockingDragOperation::DockingViaTabWell))
			{
				//we are dragging the tab through a tabwell
				//update the position of the tab that we are dragging in the panel
				this->m_child_being_dragged_offset = compute_dragged_tab_offset(my_geometry, drag_drop_event, m_tab_grab_offset_fraction);
				return FReply::handled();
			}
		}
		return FReply::un_handled();
	}
	FReply SDockingTabWell::On_Drop(const FGeometry& my_geometry, const FDragDropEvent& drag_drop_event)
	{
		std::shared_ptr<FDockingDragOperation> drag_drop_operation = drag_drop_event.get_operation_as<FDockingDragOperation>();

		if (drag_drop_operation)
		{
			if (drag_drop_operation->can_dock_in_node(m_parent_tab_stack_ptr.lock(), FDockingDragOperation::DockingViaTabWell))
			{
				//it's rare, but sometimes a drop operation can happen after we drag a tab out of a docking tab well but before the engine has a
				//chance to notify the next docking tab well what a drag operation has entered it, in this case, just use the tab referenced by the
				//drag/drop operation
				if (!m_tab_being_dragged_ptr)
				{
					m_tab_being_dragged_ptr = drag_drop_operation->get_tab_being_dragged();
				}

				if (m_tab_being_dragged_ptr)
				{
					//we dropped a tab into this tabwell
					const std::shared_ptr<SDockTab> tab_being_dragged = m_tab_being_dragged_ptr;

					//figure out where in this tabwell to drop the tab
					const int32_t drop_location_index = compute_child_drop_index(my_geometry, tab_being_dragged);

					//remove the tab when dropped, if it was being dragged in this it will be added again in a more permanent way by open tab
					m_tabs.remove(tab_being_dragged);

					//actually insert the new tab
					m_parent_tab_stack_ptr.lock()->open_tab(tab_being_dragged, drop_location_index);

					//we are no longer dragging a tab
					//note:this function is important
					m_tab_being_dragged_ptr.reset();
					//todo:implement this function

					return FReply::handled();
				}
			}
		}

		//someone just dropped something in here, but we have no idea what to do with it
		return FReply::un_handled();
	}
	glm::vec2 SDockingTabWell::Compute_Desired_Size(float) const
	{
		glm::vec2 desired_size_result(0.0f, 0.0f);

		std::shared_ptr<SDockTab> tab_being_dragged = m_tab_being_dragged_ptr;

		for (int32_t tab_index = 0; tab_index < m_tabs.num(); ++tab_index)
		{
			//currently not respecting visibility because tabs cannot be invisible
			const std::shared_ptr<SDockTab>& some_tab = m_tabs[tab_index];

			//tab being dragged is computed later
			if (some_tab != tab_being_dragged)
			{
				const glm::vec2 some_tab_desired_size = some_tab->get_desired_size();
				desired_size_result.x += some_tab_desired_size.x;
				desired_size_result.y = std::max(some_tab_desired_size.y, desired_size_result.y);//todo:fix me
			}
		}

		if (tab_being_dragged)
		{
			const glm::vec2 some_tab_desired_size = m_tab_being_dragged_ptr->get_desired_size();
			desired_size_result.x += some_tab_desired_size.x;
			desired_size_result.y = std::max(some_tab_desired_size.y, desired_size_result.y);
		}

		return desired_size_result;
	}
	FChildren* SDockingTabWell::Get_Children()
	{
		return &m_tabs;
	}
	FReply SDockingTabWell::On_Mouse_Button_On_Up(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		if (m_tab_being_dragged_ptr) //todo:fix me, this function need to call, to reset tab position
		{
			const std::shared_ptr<SDockTab> tab_being_dragged = m_tab_being_dragged_ptr;
			this->m_tab_being_dragged_ptr.reset();
			const int32_t drop_location_index = compute_child_drop_index(my_geometry, tab_being_dragged);

			//reorder the tab
			m_tabs.remove(tab_being_dragged);
			m_tabs.insert(tab_being_dragged, drop_location_index - 1);//note:to fix this function

			bring_tab_to_front(tab_being_dragged);

			//we are no longer dragging a tab in this tab well, so stop showing it in the tabwell
			return FReply::handled().release_mouse_capture();
		}
		else
		{
			return FReply::un_handled();
		}
	}
	FReply SDockingTabWell::On_Mouse_Move(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		if (this->has_mouse_capture())
		{
			//we are dragging the tab through a tab well
			//update the position of the tab that we are dragging in the panel
			this->m_child_being_dragged_offset = compute_dragged_tab_offset(my_geometry, mouse_event, m_tab_grab_offset_fraction);
			return FReply::handled();
		}
		else
		{
			return FReply::un_handled();
		}
	}
	EWindowZone::Type SDockingTabWell::get_window_zone_override() const
	{
		//pretend we are a title bar so the user can grab the area to move window around
		return EWindowZone::TitleBar;
	}

	FReply SDockingTabWell::start_dragging_tab(std::shared_ptr<SDockTab> tab_to_start_dragging,
		glm::vec2 tab_grab_offset_fraction, const FPointerEvent& mouse_event)
	{
		std::shared_ptr<FTabManager> tab_manager = tab_to_start_dragging->get_tab_manager_ptr();
		if(!tab_manager)
		{
			return FReply::handled();
		}

		const bool b_can_leave_tabwell = tab_manager->get_private_api().can_tab_leave_tab_well(tab_to_start_dragging);

		//we are about to start dragging a tab, so make sure its offset is correct
		this->m_child_being_dragged_offset = compute_dragged_tab_offset(mouse_event.find_geometry(shared_from_this()), mouse_event, tab_grab_offset_fraction);

		//the tab well keeps track of which tab we are dragging, we treat is specially during rendering and layout
		m_tab_being_dragged_ptr = tab_to_start_dragging;
		m_tab_grab_offset_fraction = tab_grab_offset_fraction;

		m_tabs.remove(tab_to_start_dragging);

		if(b_can_leave_tabwell)
		{
			//we just removed the foreground tab
			m_foreground_tab_index = -1;
			m_parent_tab_stack_ptr.lock()->on_tab_removed(tab_to_start_dragging->get_layout_identifier());

			//start dragging
			std::shared_ptr<FDockingDragOperation> drag_drop_operation =
				FDockingDragOperation::New(
						tab_to_start_dragging,
						tab_grab_offset_fraction,
						get_dock_area(),
						m_parent_tab_stack_ptr.lock()->get_tab_stack_geometry().get_local_size()
					);

			return FReply::handled().begin_drag_drop(drag_drop_operation);
		}
		else
		{
			return FReply::handled().capture_mouse(shared_from_this());
		}
	}

	float SDockingTabWell::compute_dragged_tab_offset(const FGeometry& my_geometry, const FPointerEvent& mouse_event,
		const glm::vec2& tab_grab_offset_fraction) const
	{
		const glm::vec2 computed_child_size = Compute_Child_Size(my_geometry);

		return my_geometry.absolute_to_local(mouse_event.get_screen_space_position()).x - tab_grab_offset_fraction.x * computed_child_size.x;//note:how to understand it?
	}
	int32_t SDockingTabWell::compute_child_drop_index(const FGeometry& my_geometry, const std::shared_ptr<SDockTab>& tab_being_dragged)
	{
		const float child_width = Compute_Child_Size(my_geometry).x;
		const float child_width_with_overlap = child_width - tab_being_dragged->get_overlap_width();
		const float dragged_child_center = m_child_being_dragged_offset + child_width / 2.0f;
		const int32_t drop_location_index = std::clamp(static_cast<int32_t>(dragged_child_center / child_width_with_overlap), 0, m_tabs.num());
		return drop_location_index;
	}
}
