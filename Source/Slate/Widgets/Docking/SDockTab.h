#pragma once

#include "Slate/Widgets/Layout/SBorder.h"

#include "Slate/Framework/Docking/TabManager.h"//FTabId

namespace DoDo {
	/*
	* how will this tab be used
	*/
	enum ETabRole : uint8_t
	{
		MajorTab,
		PanelTab,
		NomadTab,
		DocumentTab,
		NumRoles
	};
	/*the cause of a tab activation*/
	enum ETabActivationCause : uint8_t
	{
		UserClickedOnTab,
		SetDirectly
	};

	struct FDockTabStyle;

	//class FTabManager;
	class SDockTab : public SBorder
	{
		friend class FTabManager;
		//friend class STabSidebar;
	public:
		/*invoked when a tab is closing*/
		DECLARE_DELEGATE_OneParam(FOnTabClosedCallback, std::shared_ptr<SDockTab>)

		/*invoked when a tab is activated*/
		DECLARE_DELEGATE_TwoParams(FOnTabActivatedCallback, std::shared_ptr<SDockTab>, ETabActivationCause)

		/*invoked when a tab is renamed*/
		DECLARE_DELEGATE_OneParam(FOnTabRenamed, std::shared_ptr<SDockTab>)

		/*invoked when this tab should save some information about its content*/
		DECLARE_DELEGATE(FOnPersistVisualState)

		/*delegate called before a tab is closed, returning false will prevent the tab from closing*/
		DECLARE_DELEGATE_RetVal(bool, FCanCloseTab)

		/*invoked to add entries to the tab context menu*/
		//todo:implement FMenuBuilder

		SLATE_BEGIN_ARGS(SDockTab)
			: _Content()
			, _ContentPadding(0.0f)
			, _Label()
			, _OnTabClosed()
			, _OnTabActivated()
			, _OnTabRelocated()
			, _OnTabDraggedOverDockArea()
			, _ShouldAutosize(false)
			, _OnCanCloseTab()
			, _OnPersistVisualState()
			, _TabColorScale(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))//todo:modify to transparent
			, _ForegroundColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
			, _IconColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
			{}

			SLATE_DEFAULT_SLOT(FArguments, Content)

			SLATE_ATTRIBUTE(FMargin, ContentPadding)

			SLATE_ATTRIBUTE(DoDoUtf8String, Label)

			SLATE_EVENT(FOnTabClosedCallback, OnTabClosed)

			SLATE_EVENT(FOnTabActivatedCallback, OnTabActivated)

			SLATE_EVENT(FSimpleDelegate, OnTabRelocated)

			SLATE_EVENT(FSimpleDelegate, OnTabDraggedOverDockArea)

			SLATE_ARGUMENT(bool, ShouldAutosize)

			SLATE_EVENT(FCanCloseTab, OnCanCloseTab)

			SLATE_EVENT(FOnPersistVisualState, OnPersistVisualState)

			//SLATE_EVENT(FSimpleDelegate, OnTabDrawerClosed)//todo:how to understand it?

			SLATE_ATTRIBUTE(glm::vec4, TabColorScale)

			SLATE_ATTRIBUTE(glm::vec4, ForegroundColor)//todo:modify to FSlateColor

			SLATE_ATTRIBUTE(glm::vec4, IconColor)
		SLATE_END_ARGS()

		/*Construct the widget from the declaration*/
		void Construct(const FArguments& in_args);

		/*is this an MajorTab? A tool panel tab?*/
		ETabRole get_tab_role() const;

		/*similar to get tab role() but returns the correct role for ui style and user input purposes*/
		ETabRole get_visual_tab_role() const;

	protected:
		/*@return the style currently applied to the dock tab*/
		const FDockTabStyle& get_current_style() const;

		/*@return the padding for the tab widget*/
		FMargin get_tab_padding() const;
	protected:
		/*the tab manager that created this tab*/
		std::weak_ptr<FTabManager> m_my_tab_manager;

		/*the stuff to show when this tab is selected*/
		std::shared_ptr<SWidget> m_content;
		std::shared_ptr<SWidget> m_tab_well_content_left;
		std::shared_ptr<SWidget> m_tab_well_content_right;
		std::shared_ptr<SWidget> m_title_bar_content_right;

		/*the tab's layout identifier*/
		FTabId m_layout_identifier;

		/*Is this an MajorTab?A tool panel tab?*/
		ETabRole m_tab_role;

		/*the tab's parent tab well, null if it is a floating tab*/
		//todo:implement SDockingTabWell

		/*the label on the tab*/
		TAttribute<DoDoUtf8String> m_tab_label;

		/*the icon on the tab*/
		TAttribute<const FSlateBrush*> m_tab_icon;

		/*the styles used to draw the tab in its various states*/
		const FDockTabStyle* m_major_tab_style;
		const FDockTabStyle* m_generic_tab_style;//note:don't owner the life time

		//todo:add call back

		TAttribute<glm::vec4> m_tab_color_scale;

		/*color of this tab's icon*/
		TAttribute<glm::vec4> m_icon_color;

		//todo:implement get animated scale

		//todo:implement FCurveSequence

		/*the dock area this tab is currently being dragged over, used in nomad tabs to change style*/
		std::shared_ptr<SDockingArea> m_dragged_over_docking_area;

		//todo:implement STextBlock

		//todo:implement SImage

		//todo:implement last activation time
	};
}