#include "PreCompileHeader.h"

#include "SlateEditableTextLayout.h"

namespace DoDo {
	FSlateEditableTextLayout::FSlateEditableTextLayout(ISlateEditableTextWidget& in_owner_widget, const TAttribute<DoDoUtf8String>& in_initial_text, FTextBlockStyle in_text_style, const std::optional<ETextShapingMethod> in_text_shaping_method, const std::optional<ETextFlowDirection> in_text_flow_direction, std::shared_ptr<ITextLayoutMarshaller> in_text_marshaller)
	{
	}
	FSlateEditableTextLayout::~FSlateEditableTextLayout()
	{
	}
	FReply FSlateEditableTextLayout::handle_key_char(const FCharacterEvent& in_character_event)
	{
		//check for special characters
		const DoDoUtf8String character = in_character_event.get_character();

		uint32_t utf8_code_point = character.utf8_code_at(0);

		switch (utf8_code_point)
		{
		case u8'\u0008': //backspace
			//todo:check is text read only
			break;
		}

		return FReply::un_handled();
	}
}