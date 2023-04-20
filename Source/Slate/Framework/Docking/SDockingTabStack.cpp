#include <PreCompileHeader.h>

#include "SDockingTabStack.h"

#include "Slate/Widgets/Layout/SBorder.h"

#include "SDockingTabWell.h"

#include "Slate/Widgets/Layout/SSpacer.h"

#include "SlateCore/Widgets/Images/SImage.h"

#include "Slate/Widgets/Text/STextBlock.h"

#include "Slate/Widgets/Input/SButton.h"

#include "Application/Application.h"//get app icon depends on it

#include "SlateCore/Input/DragAndDrop.h"//FDragDropEvent depends on it

#include "FDockingDragOperation.h"

namespace DoDo {
	void SDockingTabStack::on_last_tab_removed()
	{
		//todo:check is document area

		this->set_node_content(SNullWidget::NullWidget, FDockingStackOptionalContent());
	}
	void SDockingTabStack::on_tab_removed(const FTabId& tab_id)
	{
		remove_persistent_tab(tab_id);
	}

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
		+ SOverlay::Slot()
		[
			SNew(SHorizontalBox)//todo:add visibility
			.Visibility(EVisibility::SelfHitTestInvisible)
			+ SHorizontalBox::Slot()
			.auto_width()
			.Expose(m_inline_content_area_left)
			+ SHorizontalBox::Slot()
			.fill_width(1.0f)
			.VAlign(VAlign_Bottom)
			.Padding(0.0f, 0.0f, 0.0f, 0.0f)
			[
				//todo:implement SVertical Box
				SNew(SVerticalBox)
				.Visibility(EVisibility::SelfHitTestInvisible)
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

		
		m_child_slot
		[
			SNew(SVerticalBox)
			.Visibility(EVisibility::SelfHitTestInvisible)
			+ SVerticalBox::Slot()
			
			.auto_height()
			[
				//tab well area
				SNew(SBorder)
				//.DesiredSizeScale(this, &SDockingTabStack::get_tab_well_scale)
				.BorderImage(this, &SDockingTabStack::get_tab_stack_border_image)
				//.BorderImage()
				.VAlign(VAlign_Bottom)
				.Padding(0.0f)
				[
					SNew(SVerticalBox)

					+ SVerticalBox::Slot()
					.Expose(m_title_bar_slot)
					//.fill_height(1.4f)
					.auto_height()
					+ SVerticalBox::Slot()
					//.fill_height(0.0f)
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
				SAssignNew(m_overlay_managment.m_content_area_overlay, SOverlay)

				+ SOverlay::Slot()
				[
					//content goes here
					SAssignNew(m_content_slot, SBorder) //note:this is content of this SDockTab
					.BorderImage(this, &SDockingTabStack::get_content_area_brush)
					.Padding(this, &SDockingTabStack::get_content_padding)
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
	void SDockingTabStack::open_tab(const std::shared_ptr<SDockTab>& in_tab, int32_t insert_at_location, bool b_keep_in_active)
	{
		//note:put this tab to insert_at_location
		const int32_t insert_index = open_persistent_tab(in_tab->get_layout_identifier(), insert_at_location);

		//the tab may be a nomad tab, in which case it should inherit whichever tab manager it is being put into!
		in_tab->set_tab_manager(get_dock_area()->get_tab_manager());

		const FTabId tab_id = in_tab->get_layout_identifier();

		//todo:check tab is sidebar tab

		add_tab_widget(in_tab, insert_index, b_keep_in_active);
		//todo:add on live tab added
		m_tab_well->refresh_parent_cotent();
	}
	void SDockingTabStack::set_node_content(const std::shared_ptr<SWidget>& in_content, const FDockingStackOptionalContent& optional_content)
	{
		m_content_slot->set_content(in_content);//note:this function is important, will use SDockTab's content to populate this
		(*m_inline_content_area_left)[optional_content.m_content_left];
		(*m_inline_content_area_right)[optional_content.m_content_right];

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

	const TSlotlessChildren<SDockTab>& SDockingTabStack::get_tabs() const
	{
		return m_tab_well->get_tabs();
	}

	FGeometry SDockingTabStack::get_tab_stack_geometry() const
	{
		return get_tick_space_geometry();
	}

	FMargin SDockingTabStack::get_content_padding() const
	{
		std::shared_ptr<SDockTab> foreground_tab = m_tab_well->get_foreground_tab();
		return (foreground_tab != nullptr) ? foreground_tab->get_content_padding() : FMargin(0.0f);
	}
	const FSlateBrush* SDockingTabStack::get_tab_well_brush() const
	{
		std::shared_ptr<SDockTab> foreground_tab = m_tab_well->get_foreground_tab();

		//todo:fix me, use FStyleDefaults::get no brush
		return foreground_tab ? foreground_tab->get_tab_well_brush() : FStyleDefaults::get_no_brush();
	}
	const FSlateBrush* SDockingTabStack::get_tab_stack_border_image() const
	{
		static const FSlateBrush* minor_tab_background_brush = FAppStyle::get().get_brush("Brushes.Background");

		return minor_tab_background_brush;
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
	void SDockingTabStack::show_cross()
	{
		const float dock_target_size = 32.0f;

		if (!m_overlay_managment.m_b_showing_cross)
		{
			this->get_dock_area()->show_cross();

			m_overlay_managment.m_b_showing_cross = true;
			//m_overlay_managment.m_content_area_overlay->add_slot()
		
			//todo:show cross
		}
	}
	void SDockingTabStack::hide_cross()
	{
		if (m_overlay_managment.m_b_showing_cross)
		{
			m_overlay_managment.m_b_showing_cross = false;
		}
	}
	void SDockingTabStack::clear_reserved_space()
	{
		m_b_showing_title_bar_area = false;
		m_title_bar_slot->set_padding(0.0f);//note:this clear padding is important
	}
	void SDockingTabStack::reserve_space_for_window_chrome(EChromeElement element, bool b_include_padding_for_menu_bar, bool b_only_minor_tabs)
	{
		static const float top_padding_for_menu_bar = 27.0f;
		static const float left_padding_for_icon = Application::get().get_app_icon()->get_image_size().x;
		const FMargin controls_padding = FMargin(8.0f, b_include_padding_for_menu_bar ? top_padding_for_menu_bar : 5.0f, 0.0f, 0.0f);
		const FMargin icon_padding = FMargin(b_include_padding_for_menu_bar ? left_padding_for_icon + 12.0f : 25.0f, b_only_minor_tabs ? 5.0f : 0.0f, 0.0f, 0.0f);
		
		//todo:add showing title bar area
		m_b_showing_title_bar_area = true;
		const FMargin current_padding = m_title_bar_slot->get_padding();
		switch (element)
		{
		case EChromeElement::Controls:
			m_title_bar_slot->set_padding(current_padding + controls_padding);
			break;

		case EChromeElement::Icon:
			m_title_bar_slot->set_padding(current_padding + icon_padding);
			break;

		default:
			break;
		}
	}

	SDockingNode::ECleanupRetVal SDockingTabStack::clean_up_nodes()
	{
		if (m_tab_well->get_num_tabs() > 0)
		{
			return VisibleTabsUnderNode;
		}
		else if (m_tabs.size() > 0)
		{
			set_visibility(EVisibility::Collapsed);
			return HistoryTabsUnderNode;
		}
		else
		{
			return NoTabsUnderNode;
		}
	}

	void SDockingTabStack::remove_persistent_tab(const FTabId& tab_id)
	{
		auto it = std::find_if(m_tabs.begin(), m_tabs.end(), FTabMatcher(tab_id));
		if(it != m_tabs.end())
		{
			m_tabs.erase(it);
		}
	}

	//note:put tab_id to open_location_among_active_tabs
	int32_t SDockingTabStack::open_persistent_tab(const FTabId& tab_id, int32_t open_location_among_active_tabs)
	{
		auto it = std::find_if(m_tabs.begin(), m_tabs.end(), FTabMatcher(tab_id, static_cast<ETabState::Type>(ETabState::ClosedTab | ETabState::SidebarTab)));

		int32_t existing_closed_tab_index = -1;

		if (it != m_tabs.end())
		{
			existing_closed_tab_index = it - m_tabs.begin();
		}

		if (open_location_among_active_tabs == -1) //-1 indicating put dock tab at end
		{
			if (existing_closed_tab_index != -1)
			{
				FTabManager::FTab& tab = m_tabs[existing_closed_tab_index];
				tab.m_tab_state = tab.m_side_bar_location == ESidebarLocation::none ? ETabState::OpenedTab : ETabState::SidebarTab;
				return existing_closed_tab_index;
			}
			else
			{
				//this tab was never opened in the tab stack before, add it
				m_tabs.push_back(FTabManager::FTab(tab_id, ETabState::OpenedTab));
				return m_tabs.size() - 1;
			}
		}
		else
		{
			//we need to open a tab in a specific location

			//we have the index of the open tab where to insert, but we need the index in the persistent
			//array, which is an ordered list of all tabs (both open and closed)
			int32_t open_location_in_global_list = -1;
			for (int32_t tab_index = 0, open_tab_index = 0; tab_index < m_tabs.size() && open_location_in_global_list == -1; ++tab_index)
			{
				const bool b_this_tab_is_open = (m_tabs[tab_index].m_tab_state == ETabState::OpenedTab);

				if (b_this_tab_is_open)
				{
					if (open_tab_index == open_location_among_active_tabs)
					{
						open_location_in_global_list = tab_index;
					}
					++open_tab_index;
				}
			}

			if (open_location_in_global_list == -1)
			{
				//note:insert at last element behind
				open_location_in_global_list = m_tabs.size();//todo:may be error, fix me
			}

			if (existing_closed_tab_index == -1)
			{
				//create a new tab
				m_tabs.insert(m_tabs.begin() + open_location_in_global_list, FTabManager::FTab(tab_id, ETabState::OpenedTab));

				return open_location_among_active_tabs;
			}
			else
			{
				//move the existing closed tab to the new desired location
				FTabManager::FTab tab_to_move = m_tabs[existing_closed_tab_index];
				m_tabs.erase(m_tabs.begin() + existing_closed_tab_index);

				//if the element we removed was before the insert location, subtract one since the index was shifted during the removal
				if (existing_closed_tab_index <= open_location_in_global_list)
				{
					--open_location_in_global_list;
				}

				//mark the tab opened
				tab_to_move.m_tab_state = ETabState::OpenedTab;

				m_tabs.insert(m_tabs.begin() + open_location_in_global_list, tab_to_move);
				return open_location_among_active_tabs;
			}
		}
	}

	std::shared_ptr<FTabManager::FLayoutNode> SDockingTabStack::gather_persistent_layout() const
	{
		if (m_tabs.size() > 0)
		{
			//each live tab might want to save custom visual state
			{
				const std::vector<std::shared_ptr<SDockTab>> my_tabs = this->get_tabs().as_array_copy();
				for (int32_t tab_index = 0; tab_index < my_tabs.size(); ++tab_index)
				{
					//my_tabs[tab_index]->persist_visual_state();
				}
			}

			//persist layout
			std::shared_ptr<FTabManager::FStack> persistent_stack =
				FTabManager::new_stack()
				->set_size_coefficient(this->get_size_coefficient());//todo:add set hide tab well

			std::shared_ptr<SDockTab> foreground_tab = m_tab_well->get_foreground_tab();

			if (foreground_tab)
			{
				persistent_stack->set_foreground_tab(foreground_tab->get_layout_identifier());
			}

			for (int32_t tab_index = 0; tab_index < m_tabs.size(); ++tab_index)
			{
				//we do not persist document tabs, document tabs have a valid instance id in addition to a tab type
				const bool b_is_tab_persistable = true;
				if (b_is_tab_persistable)
				{
					persistent_stack->add_tab(m_tabs[tab_index]);
				}
			}

			return persistent_stack;
		}
		else
		{
			return std::shared_ptr<FTabManager::FLayoutNode>();
		}

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
	FReply SDockingTabStack::On_Drag_Over(const FGeometry& my_geometry, const FDragDropEvent& drag_drop_event)
	{
		std::shared_ptr<FDockingDragOperation> drag_drop_operation = drag_drop_event.get_operation_as<FDockingDragOperation>();

		if (drag_drop_operation)
		{
			if (drag_drop_operation->can_dock_in_node(std::static_pointer_cast<SDockingNode>(shared_from_this()), FDockingDragOperation::DockingViaTarget))
			{
				FGeometry overlay_geometry = this->find_child_geometry(my_geometry, m_overlay_managment.m_content_area_overlay);

				if (overlay_geometry.is_under_location(drag_drop_event.get_screen_space_position()))
				{
					show_cross();
				}
				else
				{
					hide_cross();
				}

				return FReply::handled();
			}
		}

		return FReply::un_handled();
	}
	void SDockingTabStack::On_Drag_Leave(const FDragDropEvent& drag_drop_event)
	{
		if (drag_drop_event.get_operation_as<FDockingDragOperation>())
		{
			hide_cross();
		}

		//return FReply::un_handled();
	}
	FReply SDockingTabStack::On_Drop(const FGeometry& my_geometry, const FDragDropEvent& drag_drop_event)
	{
		if (drag_drop_event.get_operation_as<FDockingDragOperation>())
		{
			hide_cross();
		}

		return FReply::un_handled();
	}
}