#pragma once

#include "SlateCore/Widgets/SCompoundWidget.h"

#include "Slate/Widgets/Layout/SSplitter.h"

#include "TabManager.h"//FTabManager::FLayoutNode depends on it

namespace DoDo {

	class SDockingSplitter;
	class SDockingArea;
	//class FTabManager::FLayoutNode;
	/*
	* a node in the docking/tabbing hierarchy
	* any SDockingNode can be either a stack of tabs or a splitter
	*/
	class SDockingNode : public SCompoundWidget
	{
	public:
		enum Type
		{
			/*
			* a tab stack is collection of tabs with associated content
			* only one of the tabs is active at a time, it's content will be shown
			*/
			DockTabStack,
			/*displays multiple child SDockNodes horizontally or vertically*/
			DockSplitter,
			/*top-level dock splitters*/
			DockArea,
			/*takes up some room during layout re-arranging*/
			PlaceholderNode
		};

		/*direction relative to some dock node, for example, we may wish to say "Dock a tab right of this node"*/
		enum RelativeDirection
		{
			LeftOf,
			Above,
			RightOf,
			Below,
			Center
		};

		/*tracking for the docking cross, content, and background of the tab stack*/
		struct FOverlayManagement
		{
			FOverlayManagement()
				: m_b_showing_cross(false)
			{}

			/*the overlay widget that shows tab stack's content*/
			//todo:implement SOverlay

			/*true when we are showing the dock cross*/
			bool m_b_showing_cross;
		};

		/*@return is this dock node a tab stack, splitter or something else?*/
		virtual Type get_node_type() const = 0;

		/*
		* all dock nodes are aware of their parent dock node (unless they are a root node, a.k.a DockArea)
		* the parent is set whenever a node is inserted into a docking hierarchy
		* 
		* @param InParent set this node's parent
		*/
		//virtual void set_parent_node(std::shared_ptr<>)
		//todo:implement SDockingSplitter
		virtual void set_parent_node(std::shared_ptr<SDockingSplitter> in_parent)
		{
			m_parent_node_ptr = in_parent;
		}

		/*a tab can be removed from a stack when a user drags it away or when the user closes it*/
		enum ELayoutModification
		{
			/*tab was dragged out of the node*/
			TabRemoval_DraggedOut,
			/*tab is being closed*/
			TabRemoval_Closed,
			/*tab is being put in a sidebar*/
			TabRemoval_Sidebar,
			TabRemoval_None
		};

		/*gets the dock area that this resides in*/
		virtual std::shared_ptr<SDockingArea> get_dock_area();

		/*
		* attempt to dock the tab widget from the drag drop event next to this node
		* 
		* @param direction where relative to this direction to dock the node, e.g to the right
		* @param drag drop event drag and drop event that potentially carries
		* 
		* @return an FReply::Handled() if we suceessfully docked the content, FReply::Unhandled() otherwise
		*/
		virtual FReply on_user_attempting_dock(SDockingNode::RelativeDirection direction, const FDragDropEvent& drag_drop_event);

		/*should this node auto-size or be a percentage of its parent size, this setting is usually determined by users*/
		virtual SSplitter::ESizeRule get_size_rule() const { return SSplitter::FractionOfParent; }

		/*@return the numerator for the fraction of available space that this dock node should occupy*/
		float get_size_coefficient() const;

		/*set the coefficient size*/
		void set_size_coefficient(float in_size_coefficient);

		/*
		* recursively build up an tree of FLayoutNodes that represents the persistent layout state of this docking node and all its descendants
		* the result can be easily serialized to disk or used as history for restoring tabs to their previous locations
		* 
		* @return a pointer to persistent layout node, invalid pointer if this dock node had no tab layouts worth saving
		*/
		virtual std::shared_ptr<FTabManager::FLayoutNode> gather_persistent_layout() const = 0;

		enum ECleanupRetVal
		{
			VisibleTabsUnderNode,
			HistoryTabsUnderNode,
			NoTabsUnderNode
		};

		virtual SDockingNode::ECleanupRetVal clean_up_nodes() { return NoTabsUnderNode; }
	protected:

		/*
		 * weak reference to the parent node, it is nullptr until the node
		 * is inherited into the hierarchy, also null for root nodes (aka SDockingArea)
		 */
		std::weak_ptr<SDockingSplitter> m_parent_node_ptr;

		float m_size_coefficient;
	};
}