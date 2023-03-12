#pragma once

#include "SlateCore/Widgets/SPanel.h"

#include "SlateCore/Layout/Children.h"//TSlotlessChildren depends on it

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"

#include "Slate/Widgets/Docking/SDockTab.h"

namespace DoDo {
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
	private:
		/*the tabs in this tabwell*/
		TSlotlessChildren<SDockTab> m_tabs;

		/*a pointer to the dock node that owns this tabwell*/
		std::weak_ptr<SDockingTabStack> m_parent_tab_stack_ptr;
	};
}