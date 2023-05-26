#pragma once

#include "Slate/Widgets/Layout/SBorder.h"//SBorder

#include "SlateCore/Styling/SlateTypes.h"//FEditableTextBoxStyle depends on it

#include "Slate/Framework/SlateDelegates.h"//FOnTextCommitted depends on it

namespace DoDo {
	class SEditableText;
	class SHorizontalBox;
	class SBox;
	/*
	* editable text box widget
	*/
	class SEditableTextBox : public SBorder
	{
	public:
		SLATE_BEGIN_ARGS(SEditableTextBox)
			: _Style(&FCoreStyle::get().get_widget_style<FEditableTextBoxStyle>("NormalEditableTextBox"))
			, _Text()
			{}
			/*the styling of the text box*/
			SLATE_STYLE_ARGUMENT(FEditableTextBoxStyle, Style)
			/*sets the text content for this editable text box widget*/
			SLATE_ATTRIBUTE(DoDoUtf8String, Text)

			/*called whenever the text is committed, this happed when the user presses enter or the text box loses focus*/
			SLATE_EVENT(FOnTextCommitted, OnTextCommitted)
		SLATE_END_ARGS()

		SEditableTextBox();

		void Construct(const FArguments& in_args);

		/*see attribute style*/
		void set_style(const FEditableTextBoxStyle* in_style);

		virtual bool supports_key_board_focus() const override;
		virtual FReply On_Focus_Received(const FGeometry& my_geometry, const FFocusEvent& in_focus_event) override;

	protected:
		const FEditableTextBoxStyle* m_style;

		/*box widget that adds padding around the editable text*/
		std::shared_ptr<SBox> m_padding_box;

		/*editable text widget*/
		std::shared_ptr<SEditableText> m_editable_text;

		/*padding (overrides style)*/
		TAttribute<FMargin> m_padding_override;

		/*allows for inserting additional widgets that extend the functionality of the text box*/
		std::shared_ptr<SHorizontalBox> m_box;

		/*called when the user commits their change to the editable text control*/
		FOnTextCommitted m_on_text_committed;

	private:
		FMargin determine_padding() const { return m_padding_override.Is_Set() ? m_padding_override.Get() : m_style->m_padding; }

		/*callback when the editable text is committed*/
		void on_editable_text_committed(const DoDoUtf8String& in_text, ETextCommit::Type in_commit_type);

		/*styling: border image to draw when not hovered or focused*/
		const FSlateBrush* m_border_image_normal;

		/*styling: border image to draw when hovered*/
		const FSlateBrush* m_border_image_hovered;

		/*styling: border image to draw when focused*/
		const FSlateBrush* m_border_image_focused;

		/*styling: border image to draw when read only*/
		const FSlateBrush* m_border_image_read_only;

		/*@return border image for the text box based on the hovered and focused state*/
		const FSlateBrush* get_border_image() const;
	};
}