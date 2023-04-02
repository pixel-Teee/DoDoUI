#include <PreCompileHeader.h>

#include "SSplitter.h"

#include "SlateCore/Layout/ArrangedChildren.h"
#include "SlateCore/Rendering/DrawElements.h"//FSlateDrawElement depends on it

#include "SlateCore/Styling/SlateBrush.h"//FSlateBrush
#include "SlateCore/Styling/SlateTypes.h"

namespace DoDo {
	static int32_t Clamp(const int32_t X, const int32_t Min, const int32_t Max)
	{
		return (X < Min) ? Min : (X < Max) ? X : Max;//clamp to [Min, Max]
	}

	void SSplitter::FSlot::Construct(const FChildren& slot_owner, FSlotArguments&& in_args)
	{
		TSlotBase<FSlot>::Construct(slot_owner, std::move(in_args));

		if(in_args._Value.Is_Set())
		{
			m_size_value = std::move(in_args._Value);
		}

		if(in_args._Resizable.has_value())
		{
			m_b_is_resizable = in_args._Resizable.value();
		}

		m_min_size_value = in_args._MinSize;

		if(in_args._OnSlotResized.is_bound())
		{
			m_on_slot_resized_handler = std::move(in_args._OnSlotResized);
		}

		if(in_args._SizeRule.Is_Set())
		{
			m_sizing_rule = std::move(in_args._SizeRule);
		}
	}

	SSplitter::FSlot::FSlotArguments SSplitter::Slot()
	{
		return FSlot::FSlotArguments(std::make_unique<FSlot>());
	}

	SSplitter::FScopedWidgetSlotArguments SSplitter::add_slot(int32_t at_index)
	{
		return FScopedWidgetSlotArguments{ std::make_unique<FSlot>(), m_children, at_index };//when this destroy, to call destruction function
	}

	SSplitter::SSplitter()
		: m_children(this)
		, m_hovered_handle_index(-1)
		, m_highlighted_handle_index(*this, -1)
		, m_orientation(Orient_Horizontal)
	{
		
	}

	void SSplitter::Construct(const FArguments& in_args)
	{
		m_resize_mode = in_args._ResizeMode;
		m_physical_splitter_handle_size = in_args._PhysicalSplitterHandleSize;
		m_hit_detection_splitter_handle_size = in_args._HitDetectionSplitterHandleSize;
		m_orientation = in_args._Orientation;
		m_hovered_handle_index = -1;
		m_highlighted_handle_index.Assign(*this, in_args._HighlightedHandleIndex);
		m_b_is_resizing = false;
		m_style = in_args._Style;
		m_min_splitter_child_length = in_args._MinimumSlotHeight;
		//todo:implement add slots
		m_children.add_slots(std::move(const_cast<std::vector<FSlot::FSlotArguments>&>(in_args._Slots)));
	}

	EOrientation SSplitter::get_orientation() const
	{
		return m_orientation;
	}

	/*
	 * panels arrange their children in a space described by the allocated geometry parameter, the results of the arrangement
	 * should be returned by appending a FArrangedWidget pair for every child widget, see stack panel for an example
	 */
	void SSplitter::On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const
	{
		//todo:implement FLayoutGeometry and arrange children for layout
		//std::vector<FLayoutGeometry> layout_children =
		std::vector<FLayoutGeometry> layout_children = arrange_children_for_layout(allotted_geometry);

		//arrange the children horizontally or vertically
		for(int32_t child_index = 0; child_index < m_children.num(); ++child_index)
		{
			arranged_children.add_widget(allotted_geometry.make_child(m_children[child_index].get_widget(), layout_children[child_index]));
		}
	}

	int32_t SSplitter::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry,
	                            const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id,
	                            const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		FArrangedChildren arranged_children(EVisibility::visible);//arrange children, if we have three children, the we will have two handle
		Arrange_Children(allotted_geometry, arranged_children);//dispatch to virtual function, arrange children

		//dispatch virtual paint function
		int32_t max_layer_id = paint_arranged_children(args, arranged_children, allotted_geometry, my_culling_rect, out_draw_elements, layer_id, in_widget_style, b_parent_enabled);

		//todo:implement FSplitterStyle
		const FSlateBrush* normal_handled_brush = &m_style->m_handled_normal_brush;

		//draw the splitter above any children
		max_layer_id += 1;

		//handle index is a index into our children array, but some of those may be hidden and missing from arranged children so we need to update the index for the loop below
		auto find_arranged_handle_index = [this, &arranged_children](int32_t handle_index)
		{
			if(handle_index == -1 || arranged_children.num() == m_children.num())
			{
				return handle_index;
			}

			const std::shared_ptr<SWidget>& hovered_child_to_find = m_children[handle_index].get_widget();

			for(int32_t i = 0; i < arranged_children.num(); ++i)
			{
				if (arranged_children[i].m_widget == hovered_child_to_find)
					return i;
			}
			return -1;
		};

		int32_t arranged_hovered_handle_index = find_arranged_handle_index(m_hovered_handle_index);

		int32_t arranged_highlighted_handle_index = find_arranged_handle_index(m_highlighted_handle_index.Get());

		for(int32_t child_index = 0; child_index < arranged_children.num() - 1; ++child_index)//draw splitter, number is children - 1
		{
			const FGeometry& geometry_after_splitter = arranged_children[Clamp(child_index + 1, 0, arranged_children.num() - 1)].m_geometry;

			const float half_hit_detection_splitter_handle_size = m_hit_detection_splitter_handle_size / 2.0f;//todo:implement Round To float
			const float half_physical_splitter_handle_size = m_physical_splitter_handle_size;

			glm::vec2 handle_size;
			glm::vec2 handle_position;
			if(m_orientation == Orient_Horizontal)
			{
				handle_size = glm::vec2(m_physical_splitter_handle_size, geometry_after_splitter.m_size.y);
				handle_position = glm::vec2(-m_physical_splitter_handle_size, 0);
			}
			else
			{
				handle_size = glm::vec2(geometry_after_splitter.m_size.x, m_physical_splitter_handle_size);
				handle_position = glm::vec2(0, -m_physical_splitter_handle_size);//todo:how to understand it?
			}

			if(arranged_hovered_handle_index == child_index || arranged_highlighted_handle_index == child_index) //note:check this splitter handle?
			{
				//handle is hovered
				FSlateDrawElement::MakeBox(
					out_draw_elements,
					max_layer_id,
					geometry_after_splitter.to_paint_geometry(handle_position, handle_size, 1.0f),//note:make a offset from this geometry
					normal_handled_brush,
					should_be_enabled(b_parent_enabled) ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect,
					in_widget_style.get_color_and_opacity_tint() * m_style->m_handle_highlight_brush.m_tint_color.get_specified_color()
				);
			}
			else
			{
				//handle is hovered
				FSlateDrawElement::MakeBox(
					out_draw_elements,
					max_layer_id,
					geometry_after_splitter.to_paint_geometry(handle_position, handle_size, 1.0f),
					&m_style->m_handle_highlight_brush,//todo:modify this
					should_be_enabled(b_parent_enabled) ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect,
					in_widget_style.get_color_and_opacity_tint() * normal_handled_brush->m_tint_color.get_specified_color()
				);
			}
		}

		return max_layer_id;
	}

	FReply SSplitter::On_Mouse_Button_On_Down(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		if (mouse_event.get_effecting_button() == EKeys::LeftMouseButton && m_hovered_handle_index != -1)
		{
			m_b_is_resizing = true;
			return FReply::handled().capture_mouse(shared_from_this());
		}
		else
		{
			return FReply::un_handled();
		}
	}

	FReply SSplitter::On_Mouse_Button_On_Up(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		if (mouse_event.get_effecting_button() == EKeys::LeftMouseButton && m_b_is_resizing == true)
		{
			//todo:add on splitter finished resizing

			m_b_is_resizing = false;

			return FReply::handled().release_mouse_capture();
		}

		return FReply::un_handled();
	}

	template<EOrientation SplitterOrientation>
	static int32_t SSplitter::get_handle_being_resized_from_mouse_position(float physical_splitter_handle_size,
		float hit_detection_splitter_handle_size, glm::vec2 local_mouse_pos, const std::vector<FLayoutGeometry>& child_geometries)
	{
		const int32_t axis_index = (SplitterOrientation == Orient_Horizontal) ? 0 : 1;
		const float half_hit_detection_splitter_handle_size = (hit_detection_splitter_handle_size / 2.0f);
		const float half_physical_splitter_handle_size = (physical_splitter_handle_size / 2.0f);

		//search for the two widgets between which the cursor currently resides
		for (int32_t child_index = 1; child_index < child_geometries.size(); ++child_index)
		{
			FSlateRect prev_child_rect = child_geometries[child_index - 1].get_rect_in_parent_space();
			glm::vec2 next_child_offset = child_geometries[child_index].get_offset_in_parent_space();
			float prev_bound = prev_child_rect.get_top_left()[axis_index] + prev_child_rect.get_size()[axis_index] - half_hit_detection_splitter_handle_size + half_physical_splitter_handle_size;
			float next_bound = next_child_offset[axis_index] + half_hit_detection_splitter_handle_size - half_physical_splitter_handle_size;

			if (local_mouse_pos[axis_index] > prev_bound && local_mouse_pos[axis_index] < next_bound)
			{
				return child_index - 1;
			}
		}

		return -1;
	}

	FReply SSplitter::On_Mouse_Move(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		const glm::vec2 local_mouse_position = my_geometry.absolute_to_local(mouse_event.get_screen_space_position());

		std::vector<FLayoutGeometry> layout_children = arrange_children_for_layout(my_geometry);

		if (m_b_is_resizing)
		{
			if (!(mouse_event.get_cursor_delta() == glm::vec2(0.0f)))//note:check zero
			{
				handle_resizing_by_mouse_position(m_orientation, m_physical_splitter_handle_size, m_resize_mode, m_hovered_handle_index, local_mouse_position, m_children, layout_children);
			}

			return FReply::handled();
		}
		else
		{
			//hit test which handle we are hovering over
			int32_t previous_hovered_handle_index = m_hovered_handle_index;

			m_hovered_handle_index = (m_orientation == Orient_Horizontal)
				? get_handle_being_resized_from_mouse_position<Orient_Horizontal>(m_physical_splitter_handle_size, m_hit_detection_splitter_handle_size, local_mouse_position, layout_children)
				: get_handle_being_resized_from_mouse_position<Orient_Vertical>(m_physical_splitter_handle_size, m_hit_detection_splitter_handle_size, local_mouse_position, layout_children);

			if (m_hovered_handle_index != -1)
			{
				if (find_resizeable_slot_before_handle(m_hovered_handle_index, m_children) <= -1 || find_resizeable_slot_after_handle(m_hovered_handle_index, m_children) >= m_children.num())
				{
					m_hovered_handle_index = -1;
				}
			}

			if (m_hovered_handle_index != previous_hovered_handle_index)
			{
				//todo:add event
			}

			return FReply::un_handled();
		}
	}

	FCursorReply SSplitter::On_Cursor_Query(const FGeometry& my_geometry, const FPointerEvent& cursor_event) const
	{
		if (m_b_is_resizing)
		{
			return m_orientation == Orient_Horizontal ? FCursorReply::Cursor(EMouseCursor::ResizeLeftRight)
				: FCursorReply::Cursor(EMouseCursor::ResizeUpDown);
		}

		const glm::vec2 local_mouse_position = my_geometry.absolute_to_local(cursor_event.get_screen_space_position());

		std::vector<FLayoutGeometry> layout_children = arrange_children_for_layout(my_geometry);

		//hit test which handle we are hovering over
		const int32_t current_hovered_handle_index = (m_orientation == Orient_Horizontal) ?
			get_handle_being_resized_from_mouse_position<Orient_Horizontal>(m_physical_splitter_handle_size, m_hit_detection_splitter_handle_size, local_mouse_position, layout_children)
			: get_handle_being_resized_from_mouse_position<Orient_Vertical>(m_physical_splitter_handle_size, m_hit_detection_splitter_handle_size, local_mouse_position, layout_children);
		
		if (current_hovered_handle_index != -1 && m_children[current_hovered_handle_index].can_be_resized())
		{
			return m_orientation == Orient_Horizontal ? FCursorReply::Cursor(EMouseCursor::ResizeLeftRight) : FCursorReply::Cursor(EMouseCursor::ResizeUpDown);
		}
		else
		{
			return FCursorReply::un_handled();
		}
	}

	template<EOrientation Orientation>
	static glm::vec2 compute_desired_size_for_splitter(const float physical_splitter_handle_size, const TPanelChildren<SSplitter::FSlot>& children)
	{
		glm::vec2 my_desired_size(0.0f, 0.0f);

		int32_t num_no_collapsed = 0.0f;
		for (int32_t child_index = 0.0f; child_index < children.num(); ++child_index)
		{
			const SSplitter::FSlot& cur_slot = children[child_index];
			const EVisibility child_visibility = cur_slot.get_widget()->get_visibility();
			if (child_visibility != EVisibility::Collapsed)
			{
				++num_no_collapsed;

				glm::vec2 child_desired_size(cur_slot.get_widget()->get_desired_size());
				if (Orientation == Orient_Horizontal)
				{
					my_desired_size.x += child_desired_size.x;
					my_desired_size.y = std::max(child_desired_size.y, my_desired_size.y);
				}
				else
				{
					my_desired_size.x = std::max(child_desired_size.x, my_desired_size.x);
					my_desired_size.y += child_desired_size.y;
				}
			}
		}

		float space_needed_for_handles = std::max(0, num_no_collapsed - 1) * physical_splitter_handle_size;
		if (Orientation == Orient_Horizontal)
		{
			my_desired_size.x += space_needed_for_handles;
		}
		else
		{
			my_desired_size.y += space_needed_for_handles;
		}

		return my_desired_size;
	}

	std::vector<FLayoutGeometry> SSplitter::arrange_children_for_layout(const FGeometry& allotted_geometry) const
	{
		std::vector<FLayoutGeometry> result_geometries;
		//result_geometries.resize(m_children.num());

		const int32_t axis_index = (m_orientation == Orient_Horizontal) ? 0 : 1;

		// Splitters divide the space between their children proportionately based on size coefficients
		// the size coefficients are usually determined by a user, who grabs the handled between the child elements
		// and moves them to resize the space available to the children
		// some children are sized automatically based on their content, those children cannot be resized
		// 
		// e.g.   _____________________________________ Children
		//       /              /                  /
		//      v              v                  v
		//   + - - - - - + + - - - + + - - - - - - - - - - - - - - +
		//   |           | |       | |                             |
		//   | Child 0   | |Child1 | |  Child2                     |
		//   + - - - - - + + - - - + + - - - - - - - - - - - - - - +
		//                ^         ^
		//                 \_________\___________ Resize handles.

		int32_t num_non_collapsed_children = 0;
		int32_t num_resizeable_children = 0;
		float coefficient_total = 0;
		//some space is claimed by non-resizable elements(auto-sized elements)
		float non_resizable_space = 0;
		float min_resizable_space = 0;

		auto is_slot_collapsed = [](const FSlot& slot)
		{
			return slot.get_widget()->get_visibility() == EVisibility::Collapsed;
		};

		for(int32_t child_index = 0; child_index < m_children.num(); ++child_index)
		{
			const FSlot& cur_slot = m_children[child_index];

			if(is_slot_collapsed(cur_slot))
			{
				continue;
			}

			++num_non_collapsed_children;

			if(cur_slot.m_sizing_rule.Get() == SSplitter::SizeToContent)
			{
				non_resizable_space += cur_slot.get_widget()->get_desired_size()[axis_index];
			}
			else //SizingRule == SSplitter::FractionOfParent
			{
				min_resizable_space += std::min(m_min_splitter_child_length, cur_slot.get_min_size());
				coefficient_total += cur_slot.m_size_value.Get();
			}
		}

		/*the user-sizable children must make room for the resize handles and for auto-sized children*/
		const float space_needed_for_handles = std::max(0, num_non_collapsed_children - 1) * m_physical_splitter_handle_size;
		const float resizable_space = allotted_geometry.m_size[axis_index] - space_needed_for_handles - non_resizable_space;

		std::vector<float> slot_sizes;
		//slot_sizes.resize(m_children.num());

		float extra_required_space = 0;

		//calculate slot sizes
		for(int32_t child_index = 0; child_index < m_children.num(); ++child_index)
		{
			const FSlot& cur_slot = m_children[child_index];

			//slot is collapsed, size is automatically 0
			if(is_slot_collapsed(cur_slot))
			{
				slot_sizes.push_back(0);
				continue;
			}

			float child_space;

			if(cur_slot.m_sizing_rule.Get() == SSplitter::SizeToContent)
			{
				child_space = cur_slot.get_widget()->get_desired_size()[axis_index];
			}
			else
			{
				child_space = resizable_space * cur_slot.m_size_value.Get() / coefficient_total - extra_required_space;
				extra_required_space = 0;
			}

			//we have enough space to clamp this and steal space from earlier slots
			if(resizable_space >= min_resizable_space)
			{
				const float clamped_space = clamp_child(cur_slot, child_space);//max[slot.min_size, splitter_handle.length, child_space]
				float current_required_space = clamped_space - child_space;

				for(int32_t prev_index = child_index - 1; prev_index >= 0 && current_required_space > 0; --prev_index)
				{
					if(m_children[prev_index].m_sizing_rule.Get() == SSplitter::FractionOfParent && !is_slot_collapsed(cur_slot))
					{
						const float min_child_size = std::max(m_min_splitter_child_length, m_children[prev_index].get_min_size());
						const float available_space = slot_sizes[prev_index] - min_child_size;
						if(available_space > 0)
						{
							const float space_to_steal = std::min(available_space, current_required_space);
							slot_sizes[prev_index] -= space_to_steal;
							current_required_space -= space_to_steal;
						}
					}
				}

				if(current_required_space > 0)
				{
					//we weren't able to steal enough space from previous children, store this value to try and take it from following children
					extra_required_space = current_required_space;
				}

				child_space = clamped_space;
			}

			slot_sizes.push_back(child_space);
		}

		//create geometries and calculate offsets
		float offset = 0;
		for(int32_t child_index = 0; child_index < m_children.num(); ++child_index)
		{
			const glm::vec2 child_offset = m_orientation == Orient_Horizontal ? glm::vec2(offset, 0) : glm::vec2(0, offset);
			const glm::vec2 child_size = m_orientation == Orient_Horizontal ? glm::vec2(slot_sizes[child_index], allotted_geometry.m_size.y) : glm::vec2(allotted_geometry.m_size.x, slot_sizes[child_index]);

			result_geometries.emplace_back(FSlateLayoutTransform(child_offset), child_size);

			//advance to the next slot, if the child is collapsed, it takes up no room and does not need a splitter
			if(!is_slot_collapsed(m_children[child_index]))
			{
				offset += slot_sizes[child_index] + m_physical_splitter_handle_size;//todo:implement round to int
			}
		}

		return result_geometries;
	}

	/*
	 * @param ProposedSize a size that a child would like to be
	 *
	 * @return a size that is clamped against the minimum size allowed for children
	 */

	float SSplitter::clamp_child(const FSlot& child_slot, float proposed_size) const
	{
		float clamped_size = std::max(m_min_splitter_child_length, proposed_size);
		clamped_size = std::max(child_slot.get_min_size(), proposed_size);
		return clamped_size;
	}

	glm::vec2 SSplitter::Compute_Desired_Size(float Layout_Scale_Multiplier) const
	{
		//return glm::vec2(100.0f, 100.0f);//todo:fix me
		glm::vec2 my_desired_size = (m_orientation == Orient_Horizontal)
			? compute_desired_size_for_splitter<Orient_Horizontal>(m_physical_splitter_handle_size, m_children)
			: compute_desired_size_for_splitter<Orient_Vertical>(m_physical_splitter_handle_size, m_children);

		return my_desired_size;
	}

	FChildren* SSplitter::Get_Children()
	{
		return &m_children;
	}
	int32_t SSplitter::find_resizeable_slot_before_handle(int32_t dragged_handle, const TPanelChildren<FSlot>& children)
	{
		//resizing collapsed r autosizing slots does not make sense (their size is predetermined)
		//search out from the dragged handle to find the first non-collapsed, non-autosizing slot we can resize
		int32_t slot_before_drag_handle = dragged_handle;
		while (slot_before_drag_handle >= 0
			&& (children[slot_before_drag_handle].get_widget()->get_visibility() == EVisibility::Collapsed || !children[slot_before_drag_handle].can_be_resized()))
		{
			--slot_before_drag_handle;
		}

		return slot_before_drag_handle;
	}
	int32_t SSplitter::find_resizeable_slot_after_handle(int32_t dragged_handle, const TPanelChildren<FSlot>& children)
	{
		const int32_t num_children = children.num();

		//but the slots list does contain collapsed children! make sure that we are not resizing a collapsed slot
		//we also cannot resize auto-sizing slots
		int32_t slot_after_drag_handle = dragged_handle + 1;
		while (slot_after_drag_handle < num_children
			&& (children[slot_after_drag_handle].get_widget()->get_visibility() == EVisibility::Collapsed || !children[slot_after_drag_handle].can_be_resized()))
		{
			++slot_after_drag_handle;
		}

		return slot_after_drag_handle;
	}
	void SSplitter::find_all_resizeable_slots_after_handle(int32_t dragged_handle, const TPanelChildren<FSlot>& children, std::vector<int32_t>& out_slot_indices)
	{
		const int32_t num_children = children.num();

		for (int32_t slot_index = dragged_handle + 1; slot_index < num_children; ++slot_index)
		{
			if (children[slot_index].get_widget()->get_visibility() == EVisibility::Collapsed || !children[slot_index].can_be_resized())
			{
				continue;
			}

			out_slot_indices.push_back(slot_index);
		}
	}
	void SSplitter::handle_resizing_by_mouse_position(EOrientation orientation, const float physical_splitter_handle_size, const ESplitterResizeMode::Type resize_mode, int32_t dragged_handle, const glm::vec2& local_mouse_pos, TPanelChildren<FSlot>& children, const std::vector<FLayoutGeometry>& child_geometries)
	{
		const int32_t axis_index = (orientation == Orient_Horizontal) ? 0 : 1;

		const float handle_pos = child_geometries[dragged_handle + 1].get_local_to_parent_transform().get_translation()[axis_index] - physical_splitter_handle_size / 2.0f;//move handle pos

		float delta = local_mouse_pos[axis_index] - handle_pos;

		handle_resizing_delta(orientation, physical_splitter_handle_size, resize_mode, dragged_handle, delta, children, child_geometries);
	}
	void SSplitter::handle_resizing_delta(EOrientation orientation, const float physical_splitter_handle_size, const ESplitterResizeMode::Type resize_mode, int32_t dragged_handle, float delta, TPanelChildren<FSlot>& children, const std::vector<FLayoutGeometry>& child_geometries)
	{
		const int32_t num_children = children.num();
		const int32_t axis_index = (orientation == Orient_Horizontal) ? 0 : 1;

		//note:
		//-prev vs. next refers to the widgets in the order they are laid out (left->right, top->bottom)
		//-new vs. old refers to the old values for width/height vs. the post-resize values

		std::vector<int32_t> slots_after_drag_handle_indices;
		//slots_after_drag_handle_indices.resize(num_children);

		if (resize_mode == ESplitterResizeMode::FixedPosition)
		{
			int32_t slot_after_drag_handle = find_resizeable_slot_after_handle(dragged_handle, children);

			if (children.is_valid_index(slot_after_drag_handle))
			{
				slots_after_drag_handle_indices.push_back(slot_after_drag_handle);
			}
		}
		else if (resize_mode == ESplitterResizeMode::Fill || resize_mode == ESplitterResizeMode::FixedSize)
		{
			find_all_resizeable_slots_after_handle(dragged_handle, children, /*Out*/ slots_after_drag_handle_indices);
		}

		const int32_t num_slots_after_drag_handle = slots_after_drag_handle_indices.size();

		if (num_slots_after_drag_handle > 0)
		{
			struct FSlotInfo
			{
				FSlot* slot;
				const FLayoutGeometry* geometry;
				float new_size;
			};

			std::vector<FSlotInfo> slots_after_drag_handle;
			//slots_after_drag_handle.resize(num_slots_after_drag_handle);

			for (int32_t slot_index = 0; slot_index < num_slots_after_drag_handle; ++slot_index)//note:to get the children information and populate the array
			{
				FSlotInfo slot_info;

				slot_info.slot = &children[slots_after_drag_handle_indices[slot_index]];
				slot_info.geometry = &child_geometries[slots_after_drag_handle_indices[slot_index]];
				slot_info.new_size = clamp_child(*slot_info.slot, slot_info.geometry->get_size_in_local_space()[axis_index]);

				slots_after_drag_handle.push_back(slot_info);
			}

			//get references the prev and next children and their layout settings so that we can modify them
			const int32_t slot_before_drag_handle = find_resizeable_slot_before_handle(dragged_handle, children);

			FSlot& prev_child = children[slot_before_drag_handle];

			const FLayoutGeometry& prev_child_geom = child_geometries[slot_before_drag_handle];

			//compute the new sizes of the children
			const float prev_child_length = prev_child_geom.get_size_in_parent_space()[axis_index];
			float new_prev_child_length = clamp_child(prev_child, prev_child_length + delta);
			delta = new_prev_child_length - prev_child_length;

			//distribute the delta across the affected slots after the drag handle
			float unused_delta = delta;

			for (int32_t distribution_count = 0; distribution_count < num_slots_after_drag_handle && unused_delta != 0; ++distribution_count)
			{
				float divided_delta = resize_mode != ESplitterResizeMode::FixedSize ? unused_delta / num_slots_after_drag_handle : unused_delta;

				//reset the unused delta to keep track of any leftover space to distribute later through stretching
				unused_delta = 0;

				int32_t slot_index = 0;
				//resize only the last handle in the case of fixed size
				if (resize_mode == ESplitterResizeMode::FixedSize)
				{
					slot_index = num_slots_after_drag_handle - 1;
				}

				for (/*slot index*/; slot_index < num_slots_after_drag_handle; ++slot_index)
				{
					FSlotInfo& slot_info = slots_after_drag_handle[slot_index];
					float current_size = slot_info.new_size;
					slot_info.new_size = clamp_child(*slot_info.slot, current_size - divided_delta);

					//if one of the slots couldn't be fully adjusted by the delta due to min/max constraints then
					//the leftover delta needs to be evenly distribute to all of the other slots
					unused_delta += slot_info.new_size - (current_size - divided_delta);
				}
			}

			delta = delta - unused_delta;

			//prev child length needs to be updated, it's value has to take into account the next child's min/max restrictions
			new_prev_child_length = clamp_child(prev_child, prev_child_length + delta);

			//cells being resized are both stretch values -> redistribute the stretch coefficients proportionately
			//to match the new child sizes on the screen
			{
				float total_stretch_length = 0.0f;
				float total_stretch_coefficients = 0.0f;

				if (prev_child.m_sizing_rule.Get() == ESizeRule::FractionOfParent)
				{
					total_stretch_length = new_prev_child_length;
					total_stretch_coefficients = prev_child.m_size_value.Get();
				}

				for (int32_t slot_index = 0; slot_index < num_slots_after_drag_handle; ++slot_index)
				{
					const FSlotInfo& slot_info = slots_after_drag_handle[slot_index];

					if (slot_info.slot->m_sizing_rule.Get() == ESizeRule::FractionOfParent)
					{
						total_stretch_length += slot_info.new_size;
						total_stretch_coefficients += slot_info.slot->m_size_value.Get();
					}
				}

				auto set_size = [](FSlot& slot, float total_stretch_coefficients, float new_length, float total_length)
				{
					if (slot.m_sizing_rule.Get() == ESizeRule::FractionOfParent)
					{
						float new_fill_size = total_length > 0.0f ? (total_stretch_coefficients * (new_length / total_length)) : total_stretch_coefficients;

						if (slot.m_on_slot_resized_handler.is_bound())
						{
							slot.m_on_slot_resized_handler.execute(new_fill_size);
						}
						else
						{
							slot.m_size_value = new_fill_size;
						}
					}
					else
					{
						slot.m_on_slot_resized_handler.execute_if_bound(new_length);
					}
				};

				set_size(prev_child, total_stretch_coefficients, new_prev_child_length, total_stretch_length);

				for (int32_t slot_index = 0; slot_index < num_slots_after_drag_handle; ++slot_index)
				{
					const FSlotInfo& slot_info = slots_after_drag_handle[slot_index];
					set_size(*(slot_info.slot), total_stretch_coefficients, slot_info.new_size, total_stretch_length);
				}
			}
		}
	}
	bool SSplitter::FSlot::can_be_resized() const
	{
		const ESizeRule current_sizing_rule = m_sizing_rule.Get();

		if (current_sizing_rule == SSplitter::ESizeRule::FractionOfParent)
		{
			return !m_b_is_resizable.has_value() || m_b_is_resizable.value();
		}
		else
		{
			return m_b_is_resizable.has_value() && m_b_is_resizable.value() && m_on_slot_resized_handler.is_bound();
		}
	}
}
