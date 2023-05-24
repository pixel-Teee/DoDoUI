#pragma once

#include "Core/Misc/Attribute.h"//TAttribute depends on it

#include "SlateCore/Fonts/FontCache.h"//ETextShapingMethod depends on it

#include "Slate/Framework/Text/TextLayout.h"//ETextFlowDirection depends on it

#include "SlateCore/Input/Reply.h"//FReply depends on it

#include "SlateCore/Input/Events.h"//FCharacterEvent depends on it

namespace DoDo {
	class ISlateEditableTextWidget;
	class DoDoUtf8String;
	class FTextBlockStyle;
	class ITextLayoutMarshaller;

	/*class to handle the cached layout of SEditableText/SMultiLineEditableText by proxying around a FTextLayout*/
	class FSlateEditableTextLayout
	{
	public:
		FSlateEditableTextLayout(ISlateEditableTextWidget& in_owner_widget, const TAttribute<DoDoUtf8String>& in_initial_text, FTextBlockStyle in_text_style,
			const std::optional<ETextShapingMethod> in_text_shaping_method, const std::optional<ETextFlowDirection> in_text_flow_direction,
			std::shared_ptr<ITextLayoutMarshaller> in_text_marshaller);

		~FSlateEditableTextLayout();

		/*called to handle an on key char event from our parent widget*/
		FReply handle_key_char(const FCharacterEvent& in_character_event);
	};
}