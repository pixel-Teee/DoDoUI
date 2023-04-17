#include <PreCompileHeader.h>

#include "FDockingDragOperation.h"

#include "TabManager.h"

#include "SDockingNode.h"
#include "SDockingArea.h"
#include "Slate/Widgets/Docking/SDockTab.h"

#include "SDockingTabWell.h"

#include "Application/Application.h"//create window depends on it

#include "SlateCore/Widgets/SWindow.h"//SWindow depends on it

#include "Slate/Framework/Docking/SDockingTabStack.h"//SDockingTabStack depends on it

#ifdef WIN32
#include "Platform/Application/WindowsPlatformApplicationMisc.h"
#endif

namespace DoDo {
	/*
	 * check if the given potential parent is a parent of potential child
	 */
	static bool is_parent_widget_of(const std::shared_ptr<SWidget>& potential_parent, const std::shared_ptr<SWidget>& potential_child)
	{
		const SWidget* parent = potential_child->get_parent_widget().get();

		while(parent != nullptr)
		{
			if(potential_parent.get() == parent)
			{
				return true;
			}
			parent = parent->get_parent_widget().get();
		}

		return false;
	}

	std::shared_ptr<FDockingDragOperation> FDockingDragOperation::New(
		const std::shared_ptr<SDockTab>& in_tab_to_be_dragged, const glm::vec2 in_tab_grab_offset,
		std::shared_ptr<SDockingArea> in_tab_owner_area, const glm::vec2& owner_area_size)
	{
		const std::shared_ptr<FDockingDragOperation> operation = std::make_shared<FDockingDragOperation>(in_tab_to_be_dragged, in_tab_grab_offset, in_tab_owner_area, owner_area_size);

		return operation;
	}

	bool FDockingDragOperation::can_dock_in_node(const std::shared_ptr<SDockingNode>& dock_node,
	                                             EViaTabWell is_docking_via_tab_well) const
	{
		//todo:implement this function
		const std::shared_ptr<FTabManager> target_tab_manager = dock_node->get_dock_area()->get_tab_manager();

		const std::shared_ptr<FTabManager> tab_manager_of_origin = this->m_tab_owner_area_of_origin->get_tab_manager();

		//note:we from this drag operation to get tab, then dock to dock_node parameter

		//if this tab related SWidget is dock_node's parent, then return false
		if(is_parent_widget_of(m_tab_being_dragged->get_content(), dock_node))
		{
			return false;
		}

		if(m_tab_being_dragged->get_tab_role() == ETabRole::NomadTab)
		{
			if(is_docking_via_tab_well == FDockingDragOperation::DockingViaTabWell)
			{
				//nomad tabs can be docked in any tab well
				return true;
			}
			else
			{
				//return target_tab_manager != FGlobalTabManager::get();
				//todo:implement global tab manager
				return true;
			}
		}
		else if(m_tab_being_dragged->get_tab_role() == ETabRole::MajorTab)
		{
			//major tabs can only be stacked, they should not
			//be allowed to split areas, they are also confined to their tab manager of origin
			//the only exception is an empty area, where docking the tab should be really easy
			const bool b_tab_manager_matches = target_tab_manager == tab_manager_of_origin;
			const bool b_can_dock_in_empty_area = dock_node->get_node_type() == SDockingArea::DockArea && std::static_pointer_cast<SDockingArea>(dock_node)->get_child_nodes().size() == 0;
			return b_tab_manager_matches && (is_docking_via_tab_well == FDockingDragOperation::DockingViaTabWell || b_can_dock_in_empty_area);
		}
		else
		{
			//most commonly, tabs are confined to their tab manager of origin
			return (target_tab_manager == tab_manager_of_origin);
		}

	}

	void FDockingDragOperation::On_Dragged(const FDragDropEvent& drag_drop_event)
	{
		const bool b_previewing_target = m_hovered_dock_target.m_target_node.lock() != nullptr;//check is valid

		if (!b_previewing_target)
		{
			//the tab is being dragged, move the decorator window to match the cursor position
			glm::vec2 target_position = drag_drop_event.get_screen_space_position() - get_decorator_offset_from_cursor();

			//todo:add update morph target shape
			
			m_cursor_decorator_window->move_window_to(target_position);
		}
	}

	void FDockingDragOperation::On_Drop(bool b_drop_was_handled, const FPointerEvent& mouse_event)
	{
		const glm::vec2 window_zie = m_cursor_decorator_window->get_size_in_screen();

		m_tab_being_dragged->set_dragged_over_dock_area(nullptr);

		if (!b_drop_was_handled)
		{
			dropped_onto_nothing();
		}
		else
		{
			//the event was handled, so we have to have some window that we dropped into
			std::shared_ptr<SWindow> window_dropped_into = mouse_event.get_window();

			m_tab_owner_area_of_origin->get_tab_manager()->get_private_api().on_tab_relocated(m_tab_being_dragged, window_dropped_into);//note:this function will close window
		}

		//destroy the cursor decorator window by calling the base class implementation because we are relocating the content into a more permanent home
		FDragDropOperation::On_Drop(b_drop_was_handled, mouse_event);

		m_tab_being_dragged.reset();
	}

	/*
	 * drag test area widgets invoke this method when a drag enters them
	 *
	 * @param ThePanel that tab well that we just dragged something into
	 */
	void FDockingDragOperation::on_tab_well_entered(const std::shared_ptr<SDockingTabWell>& the_panel)
	{
		//todo:implement this
		if (is_parent_widget_of(this->m_tab_owner_area_of_origin, the_panel->get_dock_area()))
		{
			return;
		}

		//note:this function will change state

		//we just pulled the tab into some tabwell(in some docknode)
		//hide our decorator window and let the dock node handle previewing what will happen if we drop the node
		m_hovered_tab_panel_ptr = the_panel;
		//todo:add hide window
		
		m_tab_being_dragged->set_dragged_over_dock_area(the_panel->get_dock_area());
	}

	void FDockingDragOperation::on_tab_well_left(const std::shared_ptr<SDockingTabWell>& the_panel, const FGeometry& dock_node_geometry)
	{
		//move sdocktab from the_panel

		//dock node geometry is parent stack's geometry

		//we just pulled out of some dock node's tabwell
		m_hovered_tab_panel_ptr.reset();

		//show the preview window again
		m_cursor_decorator_window->resize(dock_node_geometry.get_local_size());

		m_cursor_decorator_window->show_window();

		m_cursor_decorator_window->reshape_window(dock_node_geometry.get_layout_bounding_rect());

		FCurveSequence sequence;
		//todo:add curve

		m_cursor_decorator_stack_node->open_tab(m_tab_being_dragged);

		m_tab_being_dragged->set_dragged_over_dock_area(nullptr);
	}

	std::shared_ptr<SDockTab> FDockingDragOperation::get_tab_being_dragged()
	{
		return m_tab_being_dragged;
	}

	glm::vec2 FDockingDragOperation::get_tab_grab_offset_fraction() const
	{
		return m_tab_grab_offset_fraction;
	}

	FDockingDragOperation::FDockingDragOperation(const std::shared_ptr<SDockTab>& in_tab_to_be_dragged,
		const glm::vec2 in_tab_grab_offset_fraction, std::shared_ptr<SDockingArea> in_tab_owner_area,
		const glm::vec2& owner_area_size)
			: m_tab_being_dragged(in_tab_to_be_dragged)
			, m_tab_grab_offset_fraction(in_tab_grab_offset_fraction)
			, m_tab_owner_area_of_origin(in_tab_owner_area)
			, m_last_content_size(owner_area_size)
	{
		//todo:create cursor decorator window

		//create the decorator window that we will use during this drag and drop to make the user feel like
		//they are actually dragging a piece of UI

		//start the window off hidden
		const bool b_show_immediately = false;//todo:fix this

		m_cursor_decorator_window = Application::get().add_window(SWindow::make_cursor_decorator(), b_show_immediately);

		//usually cursor decorators figure out their size automatically from content, but we will drive it
		//here because the window will reshape itself to better reflect what will happen when the user drops the tab
		m_cursor_decorator_window->set_opacity(0.45f);

		m_cursor_decorator_window->set_sizing_rule(ESizingRule::FixedSize);
		
		m_cursor_decorator_window->set_content(
			SNew(SBorder)
					 .BorderImage(FCoreStyle::get().get_brush("Docking.Background"))
					 [
						 SNew(SDockingArea, m_tab_being_dragged->get_tab_manager_ptr(), FTabManager::new_primary_area())
						 .InitialContent
						 (
							 SAssignNew(m_cursor_decorator_stack_node, SDockingTabStack, FTabManager::new_stack())
						 )
					 ]
		);

		//todo:set active tab null
	}
	FDockingDragOperation::~FDockingDragOperation()
	{
		if (m_tab_being_dragged)
		{
			dropped_onto_nothing();
		}
	}
	const glm::vec2 FDockingDragOperation::get_decorator_offset_from_cursor()
	{
		const ETabRole role_to_use = m_tab_being_dragged->get_visual_tab_role();
		const glm::vec2 tab_desired_size = m_tab_being_dragged->get_desired_size();
		const glm::vec2 max_tab_size = FDockingConstants::get_max_tab_size_for(role_to_use);

		return m_tab_grab_offset_fraction * glm::vec2(
			std::min(tab_desired_size.x, max_tab_size.x),
			std::min(tab_desired_size.y, max_tab_size.y)
		);
	}
	void FDockingDragOperation::dropped_onto_nothing()
	{
		//if we dropped the tab into an existing docknode then it would have handled the drop event
		//we are here because that didn't happen, so make a new window with a new dock node and drop the tab into that
		std::shared_ptr<FTabManager> my_tab_manager = m_tab_being_dragged->get_tab_manager_ptr();
		if (!my_tab_manager)
		{
			return;
		}

		const glm::vec2 position_to_drop = m_cursor_decorator_window->get_position_in_screen();

		const float dpi_scale = FPlatformApplicationMisc::get_dpi_scale_factor_at_point(position_to_drop.x, position_to_drop.y);

		std::shared_ptr<SWindow> new_window_parent = my_tab_manager->get_private_api().get_parent_window();

		std::shared_ptr<SWindow> new_window = SNew(SWindow)
			.Title(FGlobalTabmanager::get()->get_application_title())
			//divide out scale, it is already factored into position
			.ScreenPosition(position_to_drop / dpi_scale)
			//make room for the title bar, otherwise windows will get progressive smaller whenver you float them
			.ClientSize(SWindow::compute_window_size_for_cotent(m_cursor_decorator_window->get_size_in_screen()))
			.CreateTitleBar(false);

		std::shared_ptr<SDockingTabStack> new_dock_node;

		if (m_tab_being_dragged->get_tab_role() == ETabRole::NomadTab)
		{
			m_tab_being_dragged->set_tab_manager(FGlobalTabmanager::get());
		}

		//create a new dockarea
		std::shared_ptr<SDockingArea> new_dock_area =
			SNew(SDockingArea, my_tab_manager, FTabManager::new_primary_area())
			.ParentWindow(new_window)
			.InitialContent
			(
				SAssignNew(new_dock_node, SDockingTabStack, FTabManager::new_stack())
			);

		if (m_tab_being_dragged->get_tab_role() == ETabRole::MajorTab || m_tab_being_dragged->get_tab_role() == ETabRole::NomadTab)
		{
			std::shared_ptr<SWindow> root_window = FGlobalTabmanager::get()->get_root_window();
			if (root_window)
			{
				//we have a root window, so all major tabs are nested under it
				
			}
			else
			{
				//app tabs get put in top-level windows, they show up on the taskbar
				Application::get().add_window(new_window)->set_content(new_dock_area);
			}
		}
		else
		{
			//other tab types are placed in child windows, their life is controlled by the top-level windows
			//they do not show up on the taskbar
			if (new_window_parent)
			{

			}
			else
			{
				Application::get().add_window(new_window)->set_content(new_dock_area);
			}
		}

		//do this after the window parenting so that the window title is set correctly
		new_dock_node->open_tab(m_tab_being_dragged);

		//let every widget under this tab manager know that this tab has found a new home
		//todo:implement this
	}
}
