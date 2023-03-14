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

		const FButtonStyle* const close_button_style = &get_current_style().m_close_button_style;
		//todo:add FTextBlockStyle to the FButtonStyle

		//parent construct
		SBorder::Construct(SBorder::FArguments()
			.BorderImage(FCoreStyle::get().get_brush("Border")) //todo:implement default no brush
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
					. Padding(0, 0, 0, 0)
					[
						SNew(SBorder)//todo:add icon widget
						[
							SAssignNew(m_icon_widget, SImage)
							.ColorAndOpacity(this, &SDockTab::get_icon_color)
							.Image(this, &SDockTab::get_tab_icon)
						]
					]
				
					+ SHorizontalBox::Slot()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()//todo:add tab label and label suffix
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
					]
					+ SHorizontalBox::Slot()
					[
						SNew(SButton)//todo:add SSpacer and close button
						.ButtonStyle(close_button_style)
						.ContentPadding(FMargin(0.0f, 1.5f, 0.0f, 0.0f))//todo:add SSpacer
					]
				]
			]
		);

		//todo:implement SOverlay
	}
	bool SDockTab::is_fore_ground() const
	{
		//return m_parent_ptr.expired() ? (m_parent_ptr.lock()->get_foreground_tab() == shared_from_this()) : true;
		return true;
	}
	ETabRole SDockTab::get_tab_role() const
	{
		return m_tab_role;
	}
	ETabRole SDockTab::get_visual_tab_role() const
	{
		return get_tab_role();
	}
	glm::vec4 SDockTab::get_icon_color() const
	{
		if (!m_icon_color.Is_Set())
		{
			return glm::vec4(1.0f);
		}

		if (this->is_fore_ground() || this->is_hovered())
		{
			return m_icon_color.Get();
		}
		else //dim to 30% if not active
		{
			return m_icon_color.Get();
		}
	}
	const FSlateBrush* SDockTab::get_tab_icon() const
	{
		return m_tab_icon.Get();
	}

	std::shared_ptr<SWidget> SDockTab::get_content()
	{
		return m_content;
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
