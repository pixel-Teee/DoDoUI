#include <PreCompileHeader.h>

#include "SDockingTabStack.h"

#include "Slate/Widgets/Layout/SBorder.h"

#include "SDockingTabWell.h"

namespace DoDo {
	void SDockingTabStack::Construct(const FArguments& in_args, const std::shared_ptr<FTabManager::FStack>& persistent_node)
	{
		//todo:implement bind tab commands

		m_tabs = persistent_node->m_tabs;
		this->set_size_coefficient(persistent_node->get_size_coefficient());

		//the value of this is determined every time a tab is added
		m_b_showing_title_bar_area = false;

		//todo:add more member variable

		//todo:add curve animation

		//in tab stack mode, we glue together a tab well, inline content areas and a content overlay
		//that shows the content of the currently selected tab
		//										 _____________TabWell
		//										|
		//	+-----------------------------------v-----------------------------------------+
		//	|							+---------------------+							  |
		//	| inline content area left  | tab0 | tab1 | tab2 | inline content area right  |
		//  +-----------------------------------------------------------------------------+
		//  |																			  | 
		//  |																			  | <-- cotent area overlay
		//  +-----------------------------------------------------------------------------+

		//create inline title bar content
		//m_title_bar_content = 
		//SNew(SOverlay)
		//+ SOverlay::Slot()
		//[
		//
		//]
	}
	void SDockingTabStack::set_node_content(const std::shared_ptr<SWidget>& in_content, const FDockingStackOptionalContent& optional_content)
	{
		m_content_slot->set_content(in_content);

		if(m_tab_well->get_foreground_tab())
		{
			if (std::shared_ptr<SWindow> parent_window = m_tab_well->get_foreground_tab()->get_parent_window())
			{
				//todo:implement parent window title bar
			}
		}
	}
}