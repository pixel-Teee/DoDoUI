#include <PreCompileHeader.h>

#include "SDockingTabStack.h"

#include "Slate/Widgets/Layout/SBorder.h"

#include "SDockingTabWell.h"

#include "Slate/Widgets/Layout/SSpacer.h"

#include "SlateCore/Widgets/Images/SImage.h"

#include "Slate/Widgets/Text/STextBlock.h"

#include "Slate/Widgets/Input/SButton.h"

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

		const FButtonStyle* const unhide_tabwell_button_style = &FCoreStyle::get().get_widget_style<FButtonStyle>("Docking.UnhideTabwellButton");

		//create inline title bar content
		m_title_bar_content = 
		SNew(SOverlay)
		//+ SOverlay::Slot().Expose(m_back_ground_content_area)
		+ SOverlay::Slot()
		[
			SNew(SHorizontalBox)//todo:add visibility
			//+ SHorizontalBox::Slot() 
			//.auto_width()
			//.Expose(m_inline_content_area_left)

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

			//+ SHorizontalBox::Slot()
			//.auto_width()
			//.Expose(m_inline_content_area_right)
			//.Padding(5.0f, 0.0f, 0.0f, 0.0f)
			//.VAlign(VAlign_Center)
		];

		
		m_child_slot
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.auto_height()
			[
				//tab well area
				SNew(SBorder)
				//.DesiredSizeScale(this, &SDockingTabStack::get_tab_well_scale)
				//.BorderImage(this, &SDockingTabStack::get_tab_stack_border_image)
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
						.Image(this, &SDockingTabStack::get_tab_well_brush)
						//.Image(FAppStyle::get().get_brush("Icons.solar-system"))
					]
				]
			]
			+ SVerticalBox::Slot()
			.fill_height(1.0f)
			[
				//tab content area
				SNew(SOverlay)

				+ SOverlay::Slot()
				[
					//content goes here
					SAssignNew(m_content_slot, SBorder) //note:this is content of this SDockTab
					.BorderImage(this, &SDockingTabStack::get_content_area_brush)
					[
						SNew(STextBlock)
						.Text("empty tab!")
					]
				]
				//+ SOverlay::Slot()
				//[
				//	SNew(STextBlock)
				//	.Text("empty tab!")
				//]
				//+ SOverlay::Slot()
				//. Padding(0.0f)
				//. HAlign(HAlign_Left)
				//. VAlign(VAlign_Top)
				//[
				//	SNew(SButton)
				//	.ButtonStyle(unhide_tabwell_button_style)
				//	[
				//		//button should be big enough to show its own image
				//		SNew(SSpacer)
				//		.Size(unhide_tabwell_button_style->m_normal.m_image_size)//todo:fix me
				//	]
				//	//SNew(SImage)
				//	//.Image(FAppStyle::get().get_brush("Icons.solar-system"))//todo:fix me
				//]
			]
		];
		
	}
	void SDockingTabStack::set_node_content(const std::shared_ptr<SWidget>& in_content, const FDockingStackOptionalContent& optional_content)
	{
		m_content_slot->set_content(in_content);//note:this function is important, will use SDockTab's content to populate this

		if(m_tab_well->get_foreground_tab())
		{
			if (std::shared_ptr<SWindow> parent_window = m_tab_well->get_foreground_tab()->get_parent_window())
			{
				//todo:implement parent window title bar
				
			}
		}
	}
	void SDockingTabStack::add_tab_widget(const std::shared_ptr<SDockTab>& in_tab, int32_t at_location, bool b_keep_inactive)
	{
		m_tab_well->add_tab(in_tab, at_location, b_keep_inactive);

		//todo:hide cross
	}
	const FSlateBrush* SDockingTabStack::get_tab_well_brush() const
	{
		std::shared_ptr<SDockTab> foreground_tab = m_tab_well->get_foreground_tab();

		//todo:fix me, use FStyleDefaults::get no brush
		return foreground_tab ? foreground_tab->get_tab_well_brush() : nullptr;
	}
	void SDockingTabStack::set_parent_node(std::shared_ptr<SDockingSplitter> in_parent)
	{
		SDockingNode::set_parent_node(in_parent);

		//ok, if this docking area has a parent window, we'll assume the window was created with no title bar, and we'll
		//place the title bar widgets into our content instead!
		const std::shared_ptr<SDockingArea>& dock_area = get_dock_area();

		m_title_bar_slot->attach_widget(m_title_bar_content);
	}
	const FSlateBrush* SDockingTabStack::get_content_area_brush() const
	{
		std::shared_ptr<SDockTab> foreground_tab = m_tab_well->get_foreground_tab();

		return foreground_tab ? foreground_tab->get_content_area_brush() : FStyleDefaults::get_no_brush();
	}
	FReply SDockingTabStack::On_Mouse_Button_On_Down(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		const std::shared_ptr<SDockTab> foreground = m_tab_well->get_foreground_tab();

		//if (foreground && !foreground->is_active())
		//{
		//
		//}

		return FReply::un_handled();
	}
}