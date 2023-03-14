#include <PreCompileHeader.h>

#include "SDockingTabWell.h"

#include "FDockingDragOperation.h" //FDockingDragOperation depends on it

#include "SDockingTabStack.h"

namespace DoDo {
	SDockingTabWell::SDockingTabWell()
		: m_tabs(this)
	{
	}
	void SDockingTabWell::Construct(const FArguments& in_args)
	{
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

				//the user should see the contents of the tab that we're dragging

			}
		}
	}
}
