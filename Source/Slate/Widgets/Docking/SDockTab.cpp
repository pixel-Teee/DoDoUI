#include <PreCompileHeader.h>

#include "SDockTab.h"

#include "SlateCore/Widgets/SOverlay.h"

#include "SlateCore/Widgets/Images/SImage.h"

#include "Slate/Widgets/Colors/SComplexGradient.h"

#include "SlateCore/Widgets/SBoxPanel.h"

#include "Slate/Widgets/Input/SButton.h"

namespace DoDo
{
	void SDockTab::Construct(const FArguments& in_args)
	{
		this->m_content = in_args._Content.m_widget;//the stuff to show when this tab is selected, child widget

		//todo:implement tab will content left

		this->m_tab_label = in_args._Label;//tab label on the tab
		//todo:implement auto size
		this->m_tab_color_scale = in_args._TabColorScale;

		this->m_icon_color = in_args._IconColor;

		//parent construct
		SBorder::Construct(SBorder::FArguments()
			.BorderImage(FCoreStyle::get().get_brush("Border")) //todo:implement default brush
			.VAlign(VAlign_Bottom)
			.Padding(0.0f)
			.ForegroundColor(in_args._ForegroundColor)
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
				[
					SNew(SImage)//todo:add get image brush
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
					//todo:add image
				]
				+ SOverlay::Slot()
				[
					SNew(SHorizontalBox) //todo:fix me

					//tab icon
					+ SHorizontalBox::Slot()
					[
						SNew(SBorder)//todo:add icon widget
					]

					+ SHorizontalBox::Slot()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()//todo:add text block
					]
					+ SHorizontalBox::Slot()
					[
						SNew(SButton)//todo:add SSpacer
					]
				]
			]
		);

		//todo:implement SOverlay
	}
}
