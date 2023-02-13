#pragma once

#include "SDockingArea.h"

#include "TabManager.h"//FTab depends on it

namespace DoDo {
	/*
	* a node in the docking/tabbing hierarchy
	* a node tab stack shows a row of tabs and the content of one selected tab
	* it also supports re-arranging tabs and dragging them out of the stack
	*/
	class SDockingTabStack : public SDockingArea
	{
	public:
		void Construct(const FArguments& in_args, const std::shared_ptr<FTabManager::FStack>& persistent_node);
	private:
		/*data that persists across sessions and when the widget associated with this node is removed*/
		std::vector<FTabManager::FTab> m_tabs;
	};
}