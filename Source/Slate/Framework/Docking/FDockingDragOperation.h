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

		/*
		 * create this drag and drop content
		 *
		 * @param InTabToBeDragged the tab being dragged (dragged within or outside of a tabwell)
		 * @param InTabGrabOffset where within the tab we grabbed, so we're not dragging by the upper left of the tab
		 * @param InTabOwnerArea the dock area that owns this tab until it is relocated
		 * @param OwnerAreaSize size of the dock area at the time when we start dragging
		 */
		static std::shared_ptr<FDockingDragOperation> New(const std::shared_ptr<SDockTab>& in_tab_to_be_dragged, const glm::vec2 in_tab_grab_offset, std::shared_ptr<SDockingArea> in_tab_owner_area,
			const glm::vec2& owner_area_size);

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
	
		/*the construct is protected, so that this class can only be instantiated as a shared pointer*/
		FDockingDragOperation(const std::shared_ptr<SDockTab>& in_tab_to_be_dragged, const glm::vec2 in_tab_grab_offset_fraction, std::shared_ptr<SDockingArea> in_tab_owner_area, const glm::vec2& owner_area_size);
	protected:
		/*what is actually being dragged in this operation*/
		std::shared_ptr<SDockTab> m_tab_being_dragged;

		/*where the user grabbed the tab as a fraction of the tab's size*/
		glm::vec2 m_tab_grab_offset_fraction;

		/*the area from which we initially started dragging*/
		std::shared_ptr<SDockingArea> m_tab_owner_area_of_origin;

		/*what the size of the content was when it was last shown, the user drags splitters to set this size, it is legitimate state*/
		glm::vec2 m_last_content_size;
	};
}