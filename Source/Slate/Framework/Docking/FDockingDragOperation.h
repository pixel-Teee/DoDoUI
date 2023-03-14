#pragma once

#include "SlateCore/Input/DragAndDrop.h"

namespace DoDo {
	class SDockTab;
	class SDockingNode;
	class SDockingTabWell;
	class SDockingArea;

	/*a sample implementation of IDragDropOpation*/
	class FDockingDragOperation : public FDragDropOperation
	{
	public:
		DRAG_DROP_OPERATE_TYPE(FDockingDragOperation, FDragDropOperation)

		/*is this dock tab being placed via a tab well or via a target*/
		enum EViaTabWell
		{
			DockingViaTabWell,
			DockingViaTarget
		};

		/*checks to see if this tab can dock in this node, some tabs can only dock via the tab well*/
		bool can_dock_in_node(const std::shared_ptr<SDockingNode>& dock_node, EViaTabWell is_docking_via_tab_well) const;

		/*
		 * drag test area widgets invoke this method when a drag enters them
		 *
		 * @param ThePanel that tab well that we just dragged something into
		 */
		void on_tab_well_entered(const std::shared_ptr<SDockingTabWell>& the_panel);

		/*@return the widget being dragged*/
		std::shared_ptr<SDockTab> get_tab_being_dragged();
	protected:

		/*what is actually being dragged in this operation*/
		std::shared_ptr<SDockTab> m_tab_being_dragged;

		/*the area from which we initially started dragging*/
		std::shared_ptr<SDockingArea> m_tab_owner_area_of_origin;
	};
}