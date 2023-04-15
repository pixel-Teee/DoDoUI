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

	class SDockingTabWell;
	class STextBlock;
	class SImage;

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
			, _TabColorScale(FLinearColor::Transparent)//todo:modify to transparent
			, _ForegroundColor(FSlateColor::use_style())
			, _IconColor()
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

			SLATE_ATTRIBUTE(FLinearColor, TabColorScale)

			SLATE_ATTRIBUTE(FSlateColor, ForegroundColor)//todo:modify to FSlateColor

			SLATE_ATTRIBUTE(FLinearColor, IconColor)
		SLATE_END_ARGS()

		/*Construct the widget from the declaration*/
		void Construct(const FArguments& in_args);

		//SWidget interface
		virtual FReply On_Mouse_Button_On_Down(const FGeometry& my_geometry, const FPointerEvent& mouse_event) override;
		virtual FReply On_Drag_Detected(const FGeometry& my_geometry, const FPointerEvent& mouse_event) override;
		virtual void On_Drag_Enter(const FGeometry& my_geometry, const FDragDropEvent& drag_drop_event) override;
		//End of SWidget interface

		/*protected constructor, widgets may only be constructed via a FArguments (i.e.: SNew(SDockTab))*/
		SDockTab();

		/*set the tab manager that is controlling this tab*/
		void set_tab_manager(const std::shared_ptr<FTabManager>& in_tab_manager);

		/*
		* make this tab active in its tabwell
		* 
		* @param InActivationMethod how this tab was activated
		*/
		void active_in_parent(ETabActivationCause in_activation_cause);

		/*@return true if this tab appears active, false otherwise*/
		bool is_fore_ground() const;

		/*is this an MajorTab? A tool panel tab?*/
		ETabRole get_tab_role() const;

		/*similar to get tab role() but returns the correct role for ui style and user input purposes*/
		ETabRole get_visual_tab_role() const;

		/*@return returns the color of this tab's icon*/
		FSlateColor get_icon_color() const;

		/*gets the tab icon*/
		const FSlateBrush* get_tab_icon() const;

		/*@return the size the tab icon should be*/
		std::optional<glm::vec2> get_tab_icon_size() const;

		/*what should the content area look like for this type of tab?
		* documents, apps, and tool panels have different backgrounds
		*/
		const FSlateBrush* get_content_area_brush() const;

		/*depending on the tabs we put into the tab well, we want a different background brush*/
		const FSlateBrush* get_tab_well_brush() const;

		/*@return the content associated with this tab*/
		std::shared_ptr<SWidget> get_content();
		std::shared_ptr<SWidget> get_left_content();
		std::shared_ptr<SWidget> get_right_content();

		/*padding around the content when it is presented by the SDockingTabStack*/
		FMargin get_content_padding() const;

		/*gets this tab's layout identifier*/
		const FTabId get_layout_identifier() const;

		/*gets the dock area that this resides in*/
		std::shared_ptr<SDockingArea> get_dock_area() const;

		/*get the window in which this tab's tabmanager has placed in*/
		std::shared_ptr<SWindow> get_parent_window() const;

		/*get the tab manager currently managing this tab, note that a user move the tab between tab managers, so this return value may change*/
		std::shared_ptr<FTabManager> get_tab_manager_ptr() const;

		/*the width that this tab will overlap with side-by-side tabs*/
		float get_overlap_width() const;

		/*sets the tab's tab well parent, or resets it if nothing is passed in*/
		void set_parent(std::shared_ptr<SDockingTabWell> parent = std::shared_ptr<SDockingTabWell>());

		/*used by the drag/drop operation to signal to this tab what it is dragging over*/
		void set_dragged_over_dock_area(const std::shared_ptr<SDockingArea>& area);

	protected:
		/*provide a default tab icon*/
		void provide_default_icon(const FSlateBrush* in_default_icon);

		/*provide a default tab label in case the spawner did not set one*/
		void provide_default_label(const DoDoUtf8String& in_default_label);

		/*@return the style currently applied to the dock tab*/
		const FDockTabStyle& get_current_style() const;

		/*@return the image brush that best represents this tab's in its current state*/
		const FSlateBrush* get_image_brush() const;

		/*@return the image brush for the tab's flasher overlay*/
		const FSlateBrush* get_flash_overlay_image_brush() const;

		/*@return the padding for the tab widget*/
		FMargin get_tab_padding() const;

		/*the label on the tab*/
		DoDoUtf8String get_tab_label() const;
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
		std::weak_ptr<SDockingTabWell> m_parent_ptr;

		/*the label on the tab*/
		TAttribute<DoDoUtf8String> m_tab_label;

		/*the icon on the tab*/
		TAttribute<const FSlateBrush*> m_tab_icon;

		/*the styles used to draw the tab in its various states*/
		const FDockTabStyle* m_major_tab_style;
		const FDockTabStyle* m_generic_tab_style;//note:don't owner the life time

		TAttribute<FMargin> m_content_area_padding;
		//todo:add call back

		TAttribute<FLinearColor> m_tab_color_scale;

		/*color of this tab's icon*/
		TAttribute<FLinearColor> m_icon_color;

		//todo:implement get animated scale

		//todo:implement FCurveSequence

		/*the dock area this tab is currently being dragged over, used in nomad tabs to change style*/
		std::shared_ptr<SDockingArea> m_dragged_over_docking_area;

		//todo:implement STextBlock

		//todo:implement SImage

		/*widget used to show the label on the tab*/
		std::shared_ptr<STextBlock> m_label_widget;
		std::shared_ptr<STextBlock> m_label_suffix;

		/*widget used to show the icon on the tab*/
		std::shared_ptr<SImage> m_icon_widget;

		//todo:implement last activation time
	};
}