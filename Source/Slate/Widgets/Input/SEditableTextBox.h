#pragma once

#include "Slate/Widgets/Layout/SBorder.h"//SBorder

namespace DoDo {
	class SEditableText;
	/*
	* editable text box widget
	*/
	class SEditableTextBox : public SBorder
	{
	public:
		SLATE_BEGIN_ARGS(SEditableTextBox)
			: _Text()
			{}
			/*sets the text content for this editable text box widget*/
			SLATE_ATTRIBUTE(DoDoUtf8String, Text)
		SLATE_END_ARGS()

		SEditableTextBox();

		void Construct(const FArguments& in_args);

		virtual bool supports_key_board_focus() const override;
		virtual FReply On_Focus_Received(const FGeometry& my_geometry, const FFocusEvent& in_focus_event) override;

	protected:
		/*editable text widget*/
		std::shared_ptr<SEditableText> m_editable_text;
	};
}