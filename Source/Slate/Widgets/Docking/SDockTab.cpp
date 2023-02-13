#include <PreCompileHeader.h>

#include "SDockTab.h"

#include "SlateCore/Widgets/SOverlay.h"

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
		//SBorder::Construct(SBorder::FArguments()
		//	.BorderImage(FCoreStyle::get().get_brush("Border")) //todo:implement default brush
		//	.VAlign(VAlign_Bottom)
		//	.Padding(0.0f)
		//	.ForegroundColor(in_args._ForegroundColor)
		//	[
		//		SNew(SOverlay)
		//		+ SOverlay::Slot()
		//		[
		//			SNew(SImage)
		//		]
		//	]
		//);

		//todo:implement SOverlay
	}
}
