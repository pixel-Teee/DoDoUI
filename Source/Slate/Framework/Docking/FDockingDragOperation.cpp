#include <PreCompileHeader.h>

#include "FDockingDragOperation.h"

#include "TabManager.h"

#include "SDockingNode.h"
#include "SDockingArea.h"
#include "Slate/Widgets/Docking/SDockTab.h"

#include "SDockingTabWell.h"

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
		
		m_tab_being_dragged->set_dragged_over_dock_area(the_panel->get_dock_area());
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
	}
}
