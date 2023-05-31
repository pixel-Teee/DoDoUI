#pragma once

#include "SlateCore/Widgets/SCompoundWidget.h"

#include "SlateCore/Widgets/SBoxPanel.h"//SHorizontalBox

namespace DoDo {
	/*
	* a slate spin box resembles tranditional spin boxes in that it is a widget that provides
	* keyboard-based and mouse-based manipulation of a numeric value
	* mouse-based manipulation: drag anywhere on the spin box to change the value
	* keyboard-based manipulation: click on the spinbox to enter text mode
	*/
	template<typename NumericType>
	class SSpinBox : public SCompoundWidget
	{
	public:

		SLATE_BEGIN_ARGS(SSpinBox<NumericType>)
		{}
			/*padding to add around this widget and its internal widgets*/
			SLATE_ATTRIBUTE(FMargin, ContentPadding)
		SLATE_END_ARGS()

		/*
		* construct the widget
		* 
		* @param InArgs a declaration from which to construct the widget
		*/
		void Construct(const FArguments& in_args)
		{
			/*
			this->m_child_slot
			.Padding(in_args._ContentPadding)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.fill_width(1.0f)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Center)
				[
					SAssign(m_text_block, STextBlock)
					//todo:add more logic
				]
			]
			*/
		}
	};
}