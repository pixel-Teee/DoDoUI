#pragma once

#include "SlateCore/Widgets/SPanel.h"

#include "SlateCore/Layout/Children.h"//TSlotlessChildren depends on it

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"

#include "Slate/Widgets/Docking/SDockTab.h"

namespace DoDo {
	class FDragDropEvent;
	class SDockingTabStack;
	//class SDockTab;
	/*
	* tabwell is a panel that shows dockable tabs
	* tabs can be re-arranged and dragged out of the tab stack
	*/
	class SDockingTabWell : public SPanel
	{
	public:
		SLATE_BEGIN_ARGS(SDockingTabWell)
			: _ParentStackNode(std::shared_ptr<SDockingTabStack>(nullptr))
			{}

		SLATE_ATTRIBUTE(std::shared_ptr<SDockingTabStack>, ParentStackNode)
		SLATE_END_ARGS()

		SDockingTabWell();

		void Construct(const FArguments& in_args);

		//SWidget interface
		virtual void On_Drag_Enter(const FGeometry& my_geometry, const FDragDropEvent& drag_drop_event) override;
		//SWidget interface
	private:
		/*the tabs in this tabwell*/
		TSlotlessChildren<SDockTab> m_tabs;

		/*a pointer to the dock node that owns this tabwell*/
		std::weak_ptr<SDockingTabStack> m_parent_tab_stack_ptr;

		/*the tab being dragged through the tab well, if there is one*/
		std::shared_ptr<SDockTab> m_tab_being_dragged_ptr;
	};
}