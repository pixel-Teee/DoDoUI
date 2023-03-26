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
	class SDockingTabStack : public SDockingArea
	{
	public:
		void Construct(const FArguments& in_args, const std::shared_ptr<FTabManager::FStack>& persistent_node);

		/*set the content that the DockNode is presenting*/
		void set_node_content(const std::shared_ptr<SWidget>& in_content, const FDockingStackOptionalContent& optional_content);

		void add_tab_widget(const std::shared_ptr<SDockTab>& in_tab, int32_t at_location = -1, bool b_keep_inactive = false);

		/*depending on the tabs we put into the tab well, we want a different background brush*/
		const FSlateBrush* get_tab_well_brush() const;

		/*overridden from SDockingNode*/
		virtual void set_parent_node(std::shared_ptr<SDockingSplitter> in_parent) override;

		/*what should the content area look like for the current tab?*/
		const FSlateBrush* get_content_area_brush() const;
	public:
		/*SWidget interface*/
		virtual FReply On_Mouse_Button_On_Down(const FGeometry& my_geometry, const FPointerEvent& mouse_event) override;
	private:
		/*data that persists across sessions and when the widget associated with this node is removed*/
		std::vector<FTabManager::FTab> m_tabs;

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
}