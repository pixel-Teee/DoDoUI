#pragma once

#include "Core/Misc/Attribute.h"//TAttribute depends on it

#include "SlateCore/Fonts/FontCache.h"//ETextShapingMethod depends on it

#include "Slate/Framework/Text/TextLayout.h"//ETextFlowDirection depends on it

#include "SlateCore/Input/Reply.h"//FReply depends on it

#include "SlateCore/Input/Events.h"//FCharacterEvent depends on it

#include "SlateEditableTextTypes.h"//FCursorInfo depends on it

#include "Slate/Framework/Text/SlateTextLayout.h"//FSlateTextLayout depends on it

namespace DoDo {
	class ISlateEditableTextWidget;
	class DoDoUtf8String;
	struct FTextBlockStyle;
	class ITextLayoutMarshaller;
	class FSlateTextLayout;

	/*class to handle the cached layout of SEditableText/SMultiLineEditableText by proxying around a FTextLayout*/
	class FSlateEditableTextLayout
	{
	public:
		FSlateEditableTextLayout(ISlateEditableTextWidget& in_owner_widget, const TAttribute<DoDoUtf8String>& in_initial_text, FTextBlockStyle in_text_style,
			const std::optional<ETextShapingMethod> in_text_shaping_method, const std::optional<ETextFlowDirection> in_text_flow_direction,
			std::shared_ptr<ITextLayoutMarshaller> in_text_marshaller);

		~FSlateEditableTextLayout();

		void set_text_style(const FTextBlockStyle& in_text_style);
		const FTextBlockStyle& get_text_style() const;

		/*called to handle an on key char event from our parent widget*/
		FReply handle_key_char(const FCharacterEvent& in_character_event);

		/*called to handle a typing a character on the current selection or at the cursor position*/
		bool handle_type_char(const DoDoUtf8String& in_char);

		int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled);

		glm::vec2 Compute_Desired_Size(float layout_scale_multiplier) const;

		/*
		* sets the current editable text for this text block
		* 
		* note: doesn't update the value of bound text, nor does it call on text changed
		* 
		* @param text to set the new text to set in the internal text layout
		* @param bForce true to force the update, even if the text currently matches what's in the text layout
		* 
		* @return true if the text was updated, false if the text wasn't update
		*/
		bool set_editable_text(const DoDoUtf8String& text_to_set, const bool b_force = false);

		void cache_desired_size(float layout_scale_multiplier);

		FChildren* Get_Children();
	private:
		/*pointer to the interface for our owner widget*/
		ISlateEditableTextWidget* m_owner_widget;

		/*the marshaller used to get/set the bound text to/from the text layout*/
		std::shared_ptr<ITextLayoutMarshaller> m_marshaller;

		/*current cursor data*/
		SlateEditableTextTypes::FCursorInfo m_cursor_info;

		/*in control of the layout and wrapping of the bound text*/
		std::shared_ptr<FSlateTextLayout> m_text_layout;

		/*the text displayed in this text block*/
		TAttribute<DoDoUtf8String> m_bound_text;

		/*default style used by the text layout*/
		FTextBlockStyle m_text_style;

		/*the last known size of the widget from the previous on paint, used to recalculate wrapping*/
		glm::vec2 m_cached_size;
	};
}