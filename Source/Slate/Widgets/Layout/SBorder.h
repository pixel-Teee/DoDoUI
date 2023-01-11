#pragma once

#include "SlateCore/Widgets/SCompoundWidget.h"

#include "SlateCore/Types/SlateEnums.h"//EHorizontalAlignment

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

		//define a FArguments type, first to declare a constructor
		SLATE_BEGIN_ARGS(SBorder)
		: _Content()
		, _HAlign(HAlign_Fill)
		, _VAlign(VAlign_Fill)
		, _ContentScale(glm::vec2(1.0f, 1.0f))
		{}

		SLATE_DEFAULT_SLOT(FArguments, Content)

		SLATE_ARGUMENT(EHorizontalAlignment, HAlign)
		SLATE_ARGUMENT(EVerticalAlignment, VAlign)

		SLATE_ATTRIBUTE(glm::vec2, ContentScale)

		SLATE_END_ARGS()

		/*
		 * default constructor
		 */
		SBorder();

		/*
		 * construct this widget
		 *
		 * @param InArgs The declaration data for this widget
		 */
		void Construct(const FArguments& in_args);

	private:
		//todo:implement FSlateBrush

		//todo:implement FSlateColor

		/*flips the image if the localization's flow direction is right to left*/
		bool m_b_flip_for_right_to_left_flow_direction;
	};
}
