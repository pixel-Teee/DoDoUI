#pragma once

#include "Slate/Widgets/Layout/SBorder.h"

#include "Slate/Framework/Docking/SDockingNode.h"//SDockingNode::RelativeDirection depends on it

namespace DoDo {
	class SDockingNode;
	/*
	* targets used by docking node, when re-arranging layout, hovering over targets
	* gives the user a preview of what will happen if they drop on that target
	* dropping actually commits the layout-restructuring
	*/
	class SDockingTarget : public SBorder
	{
	public:
		SLATE_BEGIN_ARGS(SDockingTarget)
			: _OwnerNode()
			, _DockDirection(SDockingNode::LeftOf)
			{}
			/*the dock node that this target is representing, docking will occur relative to this dock node*/
			SLATE_ARGUMENT(std::shared_ptr<SDockingNode>, OwnerNode)

			/*in which direction relative to the tab stack to dock*/
			SLATE_ARGUMENT(SDockingNode::RelativeDirection, DockDirection)
		SLATE_END_ARGS()

		void Construct(const FArguments& in_args);

		//SWidget interface
		virtual void On_Drag_Enter(const FGeometry& my_geometry, const FDragDropEvent& drag_drop_event) override;

		virtual void On_Drag_Leave(const FDragDropEvent& drag_drop_event) override;

		virtual FReply On_Drop(const FGeometry& my_geometry, const FDragDropEvent& drag_drop_event) override;
		//SWidget interface

		/*@return the dock node which this target represent*/
		std::shared_ptr<SDockingNode> get_owner() const;

		/*@return the direction which we represent (relative to the OwnerTabStack)*/
		SDockingNode::RelativeDirection get_dock_direction() const;
	private:
		const FSlateBrush* get_target_image() const;

		const FSlateBrush* m_target_image;
		
		const FSlateBrush* m_target_image_hovered;

		/*the dock node relative to which we want to dock*/
		std::weak_ptr<SDockingNode> m_owner_node;

		/*the direction in which we want to dock relative to the tab stack*/
		SDockingNode::RelativeDirection m_dock_direction;

		bool m_b_is_drag_overed;
	};
}