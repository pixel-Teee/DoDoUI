#include "PreCompileHeader.h"

#include "SEditableTextBox.h"

#include "SEditableText.h"//SEditableText depends on it

#include "SlateCore/Widgets/SBoxPanel.h"//SHorizontalBox

#include "Slate/Widgets/Layout/SBox.h"//SBox depends on it

namespace DoDo {
	SEditableTextBox::SEditableTextBox()
	{
	}
	void SEditableTextBox::Construct(const FArguments& in_args)
	{
		set_style(in_args._Style);

		m_on_text_committed = in_args._OnTextCommitted;

		SBorder::Construct(SBorder::FArguments()
			.BorderImage(this, &SEditableTextBox::get_border_image)
			//.ColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
			.Padding(0.0f)
			[
				SAssignNew(m_box, SHorizontalBox)
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				[
					SAssignNew(m_padding_box, SBox)
					.Padding(this, &SEditableTextBox::determine_padding)
					.VAlign(VAlign_Center)
					[
						SAssignNew(m_editable_text, SEditableText)
						.Text(in_args._Text)
					]
				]
			]
		);
	}
	void SEditableTextBox::set_style(const FEditableTextBoxStyle* in_style)
	{
		m_style = in_style;

		if (m_style == nullptr)
		{
			FArguments defaults;
			m_style = defaults._Style;
		}

		m_border_image_normal = &m_style->m_background_image_normal;
		m_border_image_hovered = &m_style->m_background_image_hovered;
		m_border_image_focused = &m_style->m_background_image_focused;
		m_border_image_read_only = &m_style->m_background_image_read_only;
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
	void SEditableTextBox::on_editable_text_committed(const DoDoUtf8String& in_text, ETextCommit::Type in_commit_type)
	{
		//todo:check on verify text changed

		m_on_text_committed.execute_if_bound(in_text, in_commit_type);
	}
	const FSlateBrush* SEditableTextBox::get_border_image() const
	{
		//todo:check is text read only

		if (m_editable_text->has_keyboard_focus())
		{
			return m_border_image_focused;
		}
		else
		{
			if (m_editable_text->is_hovered())
			{
				return m_border_image_hovered;
			}
			else
			{
				return m_border_image_normal;
			}
		}
	}
}