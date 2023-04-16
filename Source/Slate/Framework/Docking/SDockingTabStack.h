#pragma once

#include "SDockingArea.h"

#include "TabManager.h"//FTab depends on it

#include "SlateCore/Widgets/SNullWidget.h"

#include "SlateCore/Widgets/SBoxPanel.h"//SHorizontalBox and FSlot depend on it

#include "SlateCore/Widgets/SOverlay.h"//SOverlay depends on it

namespace DoDo {
	struct FDockingStackOptionalContent
	{
		FDockingStackOptionalContent()
			: m_content_left(SNullWidget::NullWidget)
			, m_content_right(SNullWidget::NullWidget)
			, m_title_bar_content_right(SNullWidget::NullWidget)
		{}
		//content that appears on the left side of tabs in this docking stack
		std::shared_ptr<SWidget> m_content_left;

		//content that appears on the right side of tabs in this docking stack
		std::shared_ptr<SWidget> m_content_right;

		//content that appears on the right side of the title bar in the window this stack is in
		std::shared_ptr<SWidget> m_title_bar_content_right;
	};
	/*
	* a node in the docking/tabbing hierarchy
	* a node tab stack shows a row of tabs and the content of one selected tab
	* it also supports re-arranging tabs and dragging them out of the stack
	*/
	class SBorder;
	class SDockingTabWell;
	//class SHorizontalBox;
	//class SHorizontalBox::FSlot;
	class SDockingTabStack : public SDockingNode
	{
	public:
		SLATE_BEGIN_ARGS(SDockingTabStack)
		{}
		SLATE_END_ARGS()
		virtual Type get_node_type() const override
		{
			return SDockingNode::DockTabStack;
		}

		void on_last_tab_removed();

		void on_tab_removed(const FTabId& tab_id);

		void Construct(const FArguments& in_args, const std::shared_ptr<FTabManager::FStack>& persistent_node);

		void open_tab(const std::shared_ptr<SDockTab>& in_tab, int32_t insert_at_location = -1, bool b_keep_in_active = false);

		/*set the content that the DockNode is presenting*/
		void set_node_content(const std::shared_ptr<SWidget>& in_content, const FDockingStackOptionalContent& optional_content);

		void add_tab_widget(const std::shared_ptr<SDockTab>& in_tab, int32_t at_location = -1, bool b_keep_inactive = false);

		/*@return the last known geometry of this tab stack*/
		FGeometry get_tab_stack_geometry() const;

		/*how much padding to show around the content currently being presented*/
		FMargin get_content_padding() const;

		/*depending on the tabs we put into the tab well, we want a different background brush*/
		const FSlateBrush* get_tab_well_brush() const;

		/*gets the background behind the tab stack*/
		const FSlateBrush* get_tab_stack_border_image() const;

		/*overridden from SDockingNode*/
		virtual void set_parent_node(std::shared_ptr<SDockingSplitter> in_parent) override;

		/*what should the content area look like for the current tab?*/
		const FSlateBrush* get_content_area_brush() const;

		/*elements for which we might want to reserve space*/
		enum class EChromeElement
		{
			Icon,
			Controls
		};

		/*
		* add some extra padding so that the corresponding window chrome element
		* does not overlap our tabs
		*/
		void reserve_space_for_window_chrome(EChromeElement element, bool b_include_padding_for_menu_bar, bool b_only_minor_tabs);

		virtual SDockingNode::ECleanupRetVal clean_up_nodes() override;

		void remove_persistent_tab(const FTabId& tab_id);

		int32_t open_persistent_tab(const FTabId& tab_id, int32_t open_location_among_active_tabs = -1);

		virtual std::shared_ptr<FTabManager::FLayoutNode> gather_persistent_layout() const override;
	public:
		/*SWidget interface*/
		virtual FReply On_Mouse_Button_On_Down(const FGeometry& my_geometry, const FPointerEvent& mouse_event) override;
	private:
		/*data that persists across sessions and when the widget associated with this node is removed*/
		std::vector<FTabManager::FTab> m_tabs;//FTab don't have order need

		/*the borders that hold any potential inline content areas*/
		SHorizontalBox::FSlot* m_inline_content_area_left;
		SHorizontalBox::FSlot* m_inline_content_area_right;

		SVerticalBox::FSlot* m_title_bar_slot;

		SOverlay::FOverlaySlot* m_back_ground_content_area;
		
		//todo:add SVerticalBox

		std::shared_ptr<SWidget> m_title_bar_content;

		std::shared_ptr<SBorder> m_content_slot;

		/*the tab well widget show all tabs, keeps track of the selected tab, allows tab rearranging, etc.*/
		std::shared_ptr<SDockingTabWell> m_tab_well;

		/*whether or not this tab stack is part of the title bar area*/
		bool m_b_showing_title_bar_area;
	};

	struct FTabMatcher
	{
		FTabMatcher(const FTabId& in_tab_id, ETabState::Type in_tab_state = static_cast<ETabState::Type>(ETabState::ClosedTab | ETabState::OpenedTab | ETabState::SidebarTab),
			const bool in_treat_index_none_as_wild_card = true)
			: m_tab_id_to_match(in_tab_id)
			, m_required_tab_state(in_tab_state)
			, m_treat_index_none_as_wild_card(in_treat_index_none_as_wild_card)
		{}

		bool operator()(const FTabManager::FTab& candidate) const
		{
			return ((candidate.m_tab_state & m_required_tab_state) != 0) &&
				(candidate.m_tab_id.m_tab_type == m_tab_id_to_match.m_tab_type);
			//todo:add instance id check
		}

		FTabId m_tab_id_to_match;
		ETabState::Type m_required_tab_state;
		bool m_treat_index_none_as_wild_card;
	};
}