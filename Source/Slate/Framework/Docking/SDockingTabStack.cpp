#include <PreCompileHeader.h>

#include "SDockingTabStack.h"

#include "Slate/Widgets/Layout/SBorder.h"

#include "SDockingTabWell.h"

#include "Slate/Widgets/Layout/SSpacer.h"

#include "SlateCore/Widgets/Images/SImage.h"

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
		m_title_bar_content = 
		SNew(SOverlay)
		+ SOverlay::Slot().Expose(m_back_ground_content_area)
		+ SOverlay::Slot()
		[
			SNew(SHorizontalBox)//todo:add visibility
			+ SHorizontalBox::Slot() 
			.auto_width()
			.Expose(m_inline_content_area_left)

			+ SHorizontalBox::Slot()
			.fill_width(1.0f)
			.VAlign(VAlign_Bottom)
			.Padding(4.0f, 0.0f, 0.0f, 0.0f)
			[
				//todo:implement SVertical Box
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.auto_height()
				[
					//todo:implement SSpacer
					SNew(SSpacer)
					.Size(glm::vec2(0.0f, 10.0f))
				]
				+ SVerticalBox::Slot()
				. auto_height()
				[
					//tab well
					SAssignNew(m_tab_well, SDockingTabWell)
					.ParentStackNode(std::static_pointer_cast<SDockingTabStack>(shared_from_this()))
				]
			]

			+ SHorizontalBox::Slot()
			.auto_width()
			.Expose(m_inline_content_area_right)
			.Padding(5.0f, 0.0f, 0.0f, 0.0f)
			.VAlign(VAlign_Center)
		];

		/*
		m_child_slot
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.auto_height()
			[
				//tab well area
				SNew(SBorder)
				.VAlign(VAlign_Bottom)
				.Padding(0.0f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.Expose(m_title_bar_slot)
					.auto_height()

					+ SVerticalBox::Slot()
					.auto_height()
					[
						SNew(SImage)
					]
				]
			]
			+ SVerticalBox::Slot()
			.fill_height(1.0f)
			[
				//tab content area
				//SAssignNew

				//todo:implement overlay manager
			]
		];
		*/
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