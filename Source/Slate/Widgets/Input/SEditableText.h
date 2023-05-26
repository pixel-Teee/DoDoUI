#pragma once

#include "SlateCore/Widgets/SWidget.h"

#include "Slate/Widgets/Text/ISlateEditableTextWidget.h"//interface

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"

#include "Slate/Widgets/Text/SlateEditableTextLayout.h"//FSlateEditableTextLayout depends on it

#include "Slate/Widgets/Text/PlainTextLayoutMarshaller.h"//FPlainTextLayoutMarshaller depends on it

#include "SlateCore/Styling/CoreStyle.h"

#include "SlateCore/Styling/ISlateStyle.h"

#include "SlateCore/Styling/SlateTypes.h"//FCoreStyle

#include "Slate/Framework/SlateDelegates.h"//FOnTextCommitted depends on it

namespace DoDo {
	/*
	* editable text widget
	*/
	class SEditableText : public SWidget, public ISlateEditableTextWidget
	{
	public:
		SLATE_BEGIN_ARGS(SEditableText)
			: _Text()
			, _Style(&FCoreStyle::get().get_widget_style<FTextBlockStyle>("NormalText"))
			, _TextShapingMethod()
			, _TextFlowDirection()
		{}

			/*sets the text content for this editable text widget*/
			SLATE_ATTRIBUTE(DoDoUtf8String, Text)

			/*the style of the text block, which dictates the font, color*/
			SLATE_STYLE_ARGUMENT(FTextBlockStyle, Style) //todo:modify this to FTextBlockStyle

			/*which text shaping method should we use? (unset to use the default returned by get default text shaping method)*/
			SLATE_ARGUMENT(std::optional<ETextShapingMethod>, TextShapingMethod)

			/*which text flow direction should we use?(unset to use the default return by get default text flow direction)*/
			SLATE_ARGUMENT(std::optional<ETextFlowDirection>, TextFlowDirection)

			/*called whenever the text is committed, this happens when the user presses enter or the text box loses focus*/
			SLATE_EVENT(FOnTextCommitted, OnTextCommitted)
		SLATE_END_ARGS()

		SEditableText();

		~SEditableText();

		/*
		* construct this widget
		*/
		void Construct(const FArguments& in_args);

	protected:

		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements,
			int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;

		virtual FReply On_Key_Char(const FGeometry& my_geometry, const FCharacterEvent& in_character_event) override;

		virtual FReply On_Key_Down(const FGeometry& my_geometry, const FKeyEvent& in_key_event) override;

		virtual glm::vec2 Compute_Desired_Size(float Layout_Scale_Multiplier) const override;

		virtual FChildren* Get_Children() override;

		virtual void On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const override;

		virtual bool supports_key_board_focus() const override;

		virtual std::shared_ptr<SWidget> get_slate_widget() override;

		virtual void cache_desired_size(float in_layout_scale_multiplier) override;

		virtual void on_text_committed(const DoDoUtf8String& in_text, const ETextCommit::Type in_text_action) override;

	protected:
		/*text marshaller used by the editable text layout*/
		std::shared_ptr<FPlainTextLayoutMarshaller> m_plain_text_marshaller;

		/*the text layout that deals with the editable text*/
		std::unique_ptr<FSlateEditableTextLayout> m_editable_text_layout;

		/*called whenever the text is committed, this happens when the user presses enter or the text box loses focus*/
		FOnTextCommitted m_on_text_committed_callback;
	};
}