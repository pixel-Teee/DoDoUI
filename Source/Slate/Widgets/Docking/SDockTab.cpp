#include <PreCompileHeader.h>

#include "SDockTab.h"

#include "SlateCore/Widgets/SOverlay.h"

#include "Slate/Widgets/Colors/SComplexGradient.h"

#include "SlateCore/Widgets/SBoxPanel.h"

#include "Slate/Widgets/Input/SButton.h"

#include "Slate/Widgets/Text/STextBlock.h"

#include "SlateCore/Widgets/Images/SImage.h"

#include "Slate/Framework/Docking/SDockingTabWell.h"

#include "Slate/Framework/Docking/SDockingArea.h"

#include "Slate/Widgets/Layout/SSpacer.h"

#include "SlateCore/Widgets/SNullWidget.h"

namespace DoDo
{
	void SDockTab::Construct(const FArguments& in_args)
	{
		//b_can_have_children = true;//todo:fix me

		this->m_content = in_args._Content.m_widget;//the stuff to show when this tab is selected, child widget

		//todo:implement tab will content left

		this->m_tab_label = in_args._Label;//tab label on the tab
		//todo:implement auto size
		this->m_tab_color_scale = in_args._TabColorScale;

		this->m_icon_color = in_args._IconColor;

		m_major_tab_style = &FAppStyle::get().get_widget_style<FDockTabStyle>("Docking.MajorTab");
		m_generic_tab_style = &FAppStyle::get().get_widget_style<FDockTabStyle>("Docking.Tab");//todo:to register these style

		m_content_area_padding = in_args._ContentPadding;

		const FButtonStyle* const close_button_style = &get_current_style().m_close_button_style;
		//todo:add FTextBlockStyle to the FButtonStyle

		//static FLinearColor active_border_color = FAppStyle::get().get_slate_color("Docking.Tab.ActiveTabIndicatorColor").get_specified_color();
		//static FLinearColor active_border_color_transparent = FLinearColor(active_border_color.R, active_border_color.G, active_border_color.B, 0.0f);
		//static std::vector<FLinearColor> gradient_stops{ active_border_color_transparent, active_border_color, active_border_color_transparent };

		//parent construct
		SBorder::Construct(SBorder::FArguments()
			.BorderImage(FStyleDefaults::get_no_brush()) //todo:implement default no brush
			.VAlign(VAlign_Bottom)
			.Padding(0.0f)
			.ForegroundColor(in_args._ForegroundColor)
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
				[
					SNew(SImage)//todo:add get image brush
					.Image(this, &SDockTab::get_image_brush)
				]
				//overlay for active tab indication
				+ SOverlay::Slot()
				.VAlign(VAlign_Top)
				.HAlign(HAlign_Fill)
				[
					SNew(SComplexGradient)
					.DesiredSizeOverride(glm::vec2(1.0f, 1.0f))
					//.GradientColors()//todo:add gradient stops
					.Orientation(EOrientation::Orient_Vertical)
				]
				//overlay for flashing a tab for attention
				+ SOverlay::Slot()
				[
					SNew(SBorder)
					//don't allow flasher tab overlay to absorb mouse clicks
					.Padding(this, &SDockTab::get_tab_padding)
					.BorderImage(this, &SDockTab::get_flash_overlay_image_brush)
					//todo:add image
				]
				+ SOverlay::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Fill)
				[
					SNew(SHorizontalBox) //todo:fix me
				
					//tab icon
					+ SHorizontalBox::Slot()
					. auto_width()
					. VAlign(VAlign_Center)
					. Padding(0, 0, 5, 0)
					[
						SNew(SBorder)//todo:add icon widget
						[
							SAssignNew(m_icon_widget, SImage)
							.ColorAndOpacity(this, &SDockTab::get_icon_color)
							.Image(this, &SDockTab::get_tab_icon)
							.DesiredSizeOverride(this, &SDockTab::get_tab_icon_size)
						]
					]
					+ SHorizontalBox::Slot()//todo:add tab label and label suffix
					. fill_width(1.0f)
					. Padding(0.0f, 1.0f)
					. HAlign(HAlign_Left)
					. VAlign(VAlign_Center)
					[
						//label sub hbox
						SNew(SHorizontalBox)
						//tab label
						+ SHorizontalBox::Slot()
						. fill_width(1.0f)
						. Padding(0.0f, 1.0f)
						. VAlign(VAlign_Center)
						[
							SAssignNew(m_label_widget, STextBlock)
							.TextStyle(&get_current_style().m_tab_text_style)
							.Text(this, &SDockTab::get_tab_label)
						]
					]
					+ SHorizontalBox::Slot()
					. auto_width() //note:use desired size
					. HAlign(HAlign_Right)
					. VAlign(VAlign_Center)
					[
						SNew(SButton)//todo:add SSpacer and close button
						.ButtonStyle(close_button_style)
						.ContentPadding(FMargin(0.0f, 1.5f, 0.0f, 0.0f))//todo:add SSpacer
						[
							SNew(SSpacer)
							.Size(close_button_style->m_normal.m_image_size)
						]
					]
				]
			]
		);

		//todo:implement SOverlay
	}

	static float total_dragged_distance = 0.0f;

	FReply SDockTab::On_Mouse_Button_On_Down(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		if (!has_mouse_capture())
		{
			if (mouse_event.get_effecting_button() == EKeys::LeftMouseButton)
			{
				total_dragged_distance = 0.0;
				active_in_parent(ETabActivationCause::UserClickedOnTab);

				//todo:implement detect drag
				//note:process reply will pass this widget to start_drag_detection function
				return FReply::handled().detect_drag(shared_from_this(), EKeys::LeftMouseButton);
			}//todo:add middle mouse button handle
			else if(mouse_event.get_effecting_button() == EKeys::RightMouseButton)
			{
				//we clicked on the tab, so it should be active
				active_in_parent(ETabActivationCause::UserClickedOnTab);

				return FReply::un_handled();
			}
		}

		return FReply::un_handled();
	}

	FReply SDockTab::On_Drag_Detected(const FGeometry& my_geometry, const FPointerEvent& mouse_event) //create a payload
	{
		//need to remember where within a tab we grabbed
		const glm::vec2 tab_grab_offset = my_geometry.absolute_to_local(mouse_event.get_screen_space_position());

		const glm::vec2 tab_size = my_geometry.get_local_size();

		const glm::vec2 tab_grab_offset_faction = glm::vec2(
			std::clamp(tab_grab_offset.x / tab_size.x, 0.0f, 1.0f),
			std::clamp(tab_grab_offset.y / tab_size.y, 0.0f, 1.0f)
		);

		if(std::shared_ptr<SDockingTabWell> pinned_parent = m_parent_ptr.lock())
		{
			//see if we can drag tabs contain in this manager
			std::shared_ptr<FTabManager> tab_manager = get_tab_manager_ptr();
			if(tab_manager && tab_manager->get_can_do_drag_operation())
			{
				return pinned_parent->start_dragging_tab(std::static_pointer_cast<SDockTab>(shared_from_this()), tab_grab_offset_faction, mouse_event);
			}
			else
			{
				return FReply::handled();
			}
		}
		else
		{
			//should never get here (but sometimes does)
			return FReply::un_handled();
		}
	}

	SDockTab::SDockTab()
		: m_content(SNew(SSpacer))
		, m_tab_well_content_left(SNullWidget::NullWidget)
		, m_tab_well_content_right(SNullWidget::NullWidget)
		, m_title_bar_content_right(SNullWidget::NullWidget)
		, m_layout_identifier("")
		, m_tab_role(ETabRole::PanelTab)
		, m_parent_ptr()
		, m_tab_label("DockTab")
		, m_content_area_padding(2.0f)
		, m_tab_color_scale()//todo:add more initialize
	{
	}
	void SDockTab::active_in_parent(ETabActivationCause in_activation_cause)
	{
		std::shared_ptr<SDockingTabWell> parent_tab_well = m_parent_ptr.lock();

		if (parent_tab_well)
		{
			parent_tab_well->bring_tab_to_front(std::static_pointer_cast<SDockTab>(shared_from_this()));
		}

		//todo:implement on tab activated delegate
	}
	bool SDockTab::is_fore_ground() const
	{
		return m_parent_ptr.lock() != nullptr ? (m_parent_ptr.lock()->get_foreground_tab() == shared_from_this()) : true;
		//return true;
	}
	ETabRole SDockTab::get_tab_role() const
	{
		return m_tab_role;
	}
	ETabRole SDockTab::get_visual_tab_role() const
	{
		return get_tab_role();
	}
	FSlateColor SDockTab::get_icon_color() const
	{
		if (!m_icon_color.Is_Set())
		{
			return FSlateColor::use_foreground();
		}

		if (this->is_fore_ground() || this->is_hovered())
		{
			return m_icon_color.Get();
		}
		else //dim to 30% if not active
		{
			return m_icon_color.Get().copy_with_new_opacity(0.7f);
		}
	}
	const FSlateBrush* SDockTab::get_tab_icon() const
	{
		return m_tab_icon.Get();
	}

	std::optional<glm::vec2> SDockTab::get_tab_icon_size() const
	{
		return get_current_style().m_icon_size;
	}

	const FSlateBrush* SDockTab::get_content_area_brush() const
	{
		return &get_current_style().m_content_area_brush;
	}

	const FSlateBrush* SDockTab::get_tab_well_brush() const
	{
		return &get_current_style().m_tab_well_brush;
	}

	std::shared_ptr<SWidget> SDockTab::get_content()
	{
		return m_content;
	}

	std::shared_ptr<SWidget> SDockTab::get_left_content()
	{
		return m_tab_well_content_left;
	}

	std::shared_ptr<SWidget> SDockTab::get_right_content()
	{
		return m_title_bar_content_right;
	}

	FMargin SDockTab::get_content_padding() const
	{
		return m_content_area_padding.Get();
	}

	const FTabId SDockTab::get_layout_identifier() const
	{
		return m_layout_identifier;
	}

	std::shared_ptr<SDockingArea> SDockTab::get_dock_area() const
	{
		return m_parent_ptr.expired() ? m_parent_ptr.lock()->get_dock_area() : std::shared_ptr<SDockingArea>();
	}

	std::shared_ptr<SWindow> SDockTab::get_parent_window() const
	{
		std::shared_ptr<SDockingArea> docking_area_ptr = this->get_dock_area();

		return docking_area_ptr ? docking_area_ptr->get_parent_window() : std::shared_ptr<SWindow>();
	}

	std::shared_ptr<FTabManager> SDockTab::get_tab_manager_ptr() const
	{
		return m_my_tab_manager.lock();
	}

	float SDockTab::get_overlap_width() const
	{
		return get_current_style().m_overlap_width;
	}

	void SDockTab::set_parent(std::shared_ptr<SDockingTabWell> parent)
	{
		m_parent_ptr = parent;
		//todo:implement on parent set
	}

	void SDockTab::provide_default_icon(const FSlateBrush* in_default_icon)
	{
		const bool user_provided_icon = m_tab_icon.Is_Bound() || (m_tab_icon.Get() && m_tab_icon.Get() != FStyleDefaults::get_no_brush());
		if (!user_provided_icon)
		{
			m_tab_icon = in_default_icon;
		}
	}

	void SDockTab::provide_default_label(const DoDoUtf8String& in_default_label)
	{
		bool user_provided_label = m_tab_label.Is_Bound() && m_tab_label.Get().get_length();//todo:implement is_empty

		if(!user_provided_label)//todo:fix me
		{
			m_tab_label = in_default_label;
		}
	}

	const FDockTabStyle& SDockTab::get_current_style() const
	{
		if (get_visual_tab_role() == ETabRole::MajorTab)
		{
			return *m_major_tab_style;
		}

		return *m_generic_tab_style;
	}
	const FSlateBrush* SDockTab::get_image_brush() const
	{
		const FDockTabStyle& current_style = get_current_style();

		//pick the right brush based on whether the tab is active or hovered
		if (this->is_fore_ground())
		{
			return &current_style.m_foreground_brush;
		}
		else if (this->is_hovered())
		{
			return &current_style.m_hovered_brush;
		}

		return &current_style.m_normal_brush;
	}
	const FSlateBrush* SDockTab::get_flash_overlay_image_brush() const
	{
		return nullptr;//todo:implement FStyleDefaults and flash curve
	}
	FMargin SDockTab::get_tab_padding() const
	{
		FMargin new_padding = get_current_style().m_tab_padding;

		return new_padding;
	}
	DoDoUtf8String SDockTab::get_tab_label() const
	{
		return m_tab_label.Get();
	}
}
