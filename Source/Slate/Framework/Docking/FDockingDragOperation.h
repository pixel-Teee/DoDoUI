#pragma once

#include "SlateCore/Input/DragAndDrop.h"

#include "Slate/Framework/Docking/SDockingNode.h"//RelativeDirection

namespace DoDo {
	class SDockTab;
	class SDockingNode;
	class SDockingTabWell;
	class SDockingArea;
	class SDockingTabStack;
	
	/*a sample implementation of IDragDropOpation*/
	class FDockingDragOperation : public FDragDropOperation
	{
	public:
		DRAG_DROP_OPERATE_TYPE(FDockingDragOperation, FDragDropOperation)

		/*
		* represents a target for the user re-arranging some layout
		* a user expresses their desire to re-arrange layout by placing a tab relative to some layout node
		* e.g. i want my tab left of the viewport tab
		*/
		struct FDockTarget
		{
			FDockTarget()
				: m_target_node()
				, m_dock_direction()
			{

			}

			FDockTarget(const std::shared_ptr<SDockingNode>& in_target_node, SDockingNode::RelativeDirection in_dock_direction)
				: m_target_node(in_target_node)
				, m_dock_direction(in_dock_direction)
			{}

			bool operator==(const FDockTarget& other)
			{
				return this->m_target_node.lock() == other.m_target_node.lock() &&
					this->m_dock_direction == other.m_dock_direction;
			}

			bool operator!=(const FDockTarget& other)
			{
				return !((*this) == other);
			}
			/*we'll put the tab relative to this node*/
			std::weak_ptr<SDockingNode> m_target_node;

			/*relation to node where we will put the tab*/
			SDockingNode::RelativeDirection m_dock_direction;
		};

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
		* called when the mouse was moved during a drag and drop operation
		* 
		* @param DragDropEvent the event that descibes this drag drop operation
		*/
		virtual void On_Dragged(const FDragDropEvent& drag_drop_event) override;

		/*
		* invoked when the drag and drop operation has ended
		* 
		* @param bDropWasHandled true when the drop was handled by some widget, false otherwise
		*/
		virtual void On_Drop(bool b_drop_was_handled, const FPointerEvent& mouse_event) override;

		/*
		 * drag test area widgets invoke this method when a drag enters them
		 *
		 * @param ThePanel that tab well that we just dragged something into
		 */
		void on_tab_well_entered(const std::shared_ptr<SDockingTabWell>& the_panel);

		/*
		* drag test area widgets invoke this method when a drag leaves them
		* 
		* @param ThePanel that tab well that we just dragged something out of
		*/
		void on_tab_well_left(const std::shared_ptr<SDockingTabWell>& the_panel, const FGeometry& dock_node_geometry);

		/*@return the widget being dragged*/
		std::shared_ptr<SDockTab> get_tab_being_dragged();

		/*@return location where the user grabbed within the tab as a fraction of the tab's size*/
		glm::vec2 get_tab_grab_offset_fraction() const;
	
		/*the construct is protected, so that this class can only be instantiated as a shared pointer*/
		FDockingDragOperation(const std::shared_ptr<SDockTab>& in_tab_to_be_dragged, const glm::vec2 in_tab_grab_offset_fraction, std::shared_ptr<SDockingArea> in_tab_owner_area, const glm::vec2& owner_area_size);

		virtual ~FDockingDragOperation();
	protected:
		/*@return the offset into the tab where the user grabbed in slate units*/
		const glm::vec2 get_decorator_offset_from_cursor();

		/*the tab was dropped onto nothing or someone interrupted the drag drop operation*/
		void dropped_onto_nothing();

		/*what is actually being dragged in this operation*/
		std::shared_ptr<SDockTab> m_tab_being_dragged;

		/*where the user grabbed the tab as a fraction of the tab's size*/
		glm::vec2 m_tab_grab_offset_fraction;

		/*decorator widget where we add temp doc tabs to*/
		std::shared_ptr<SDockingTabStack> m_cursor_decorator_stack_node;

		/*the area from which we initially started dragging*/
		std::shared_ptr<SDockingArea> m_tab_owner_area_of_origin;

		/*the tabwell over which we are currently hovering*/
		std::weak_ptr<SDockingTabWell> m_hovered_tab_panel_ptr;

		/*some target dock node over which we are currently hovering, it could be a tab stack or dock area*/
		FDockTarget m_hovered_dock_target;

		/*what the size of the content was when it was last shown, the user drags splitters to set this size, it is legitimate state*/
		glm::vec2 m_last_content_size;
	};
}