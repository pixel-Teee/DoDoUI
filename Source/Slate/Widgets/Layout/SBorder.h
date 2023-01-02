#pragma once

#include "SlateCore/Widgets/SCompoundWidget.h"

namespace DoDo
{
	/*
	 * a border is an widget that can be used to contain other widgets
	 * it has a border image property, which allows it to take an different appearances
	 * border also has a content() slot as well as some parameters controlling the arrangement of said content
	 */
	class SBorder : public SCompoundWidget
	{
		SLATE_DECLARE_WIDGET(SBorder, SCompoundWidget)
	public:

	private:
		//todo:implement FSlateBrush

		//todo:implement FSlateColor

		/*flips the image if the localization's flow direction is right to left*/
		bool m_b_flip_for_right_to_left_flow_direction;
	};
}