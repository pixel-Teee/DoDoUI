#include "PreCompileHeader.h"

#include "SEditableTextBox.h"

#include "SEditableText.h"//SEditableText depends on it

namespace DoDo {
	SEditableTextBox::SEditableTextBox()
	{
	}
	void SEditableTextBox::Construct(const FArguments& in_args)
	{
		SBorder::Construct(SBorder::FArguments()
			.ColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
			[
				SAssignNew(m_editable_text, SEditableText)
				.Text(in_args._Text)
			]
		);
	}
	bool SEditableTextBox::supports_key_board_focus() const
	{
		return std::static_pointer_cast<SWidget>(m_editable_text)->supports_key_board_focus();
	}
	FReply SEditableTextBox::On_Focus_Received(const FGeometry& my_geometry, const FFocusEvent& in_focus_event)
	{
		FReply reply = FReply::handled();

		if (in_focus_event.get_cause() != EFocusCause::Cleared)
		{
			//forward keyboard focus to our editable text widget
			reply.set_user_focus(m_editable_text, in_focus_event.get_cause());
		}

		return reply;
	}
}