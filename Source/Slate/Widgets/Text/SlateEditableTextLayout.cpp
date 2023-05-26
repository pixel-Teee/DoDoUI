#include "PreCompileHeader.h"

#include "SlateEditableTextLayout.h"

#include "Slate/Widgets/Text/PlainTextLayoutMarshaller.h"//ITextLayoutMarshaller depends on it

#include "Slate/Widgets/Input/SEditableText.h"

namespace DoDo {
	namespace {
		FReply bool_to_reply(const bool b_handled)
		{
			return (b_handled) ? FReply::handled() : FReply::un_handled();
		}
	}
	FSlateEditableTextLayout::FSlateEditableTextLayout(ISlateEditableTextWidget& in_owner_widget, const TAttribute<DoDoUtf8String>& in_initial_text, FTextBlockStyle in_text_style, const std::optional<ETextShapingMethod> in_text_shaping_method, const std::optional<ETextFlowDirection> in_text_flow_direction, std::shared_ptr<ITextLayoutMarshaller> in_text_marshaller)
	{
		m_text_layout = FSlateTextLayout::Create(in_owner_widget.get_slate_widget().get(), in_text_style);

		m_owner_widget = &in_owner_widget;

		m_marshaller = in_text_marshaller;

		m_text_style = in_text_style;

		m_bound_text = in_initial_text;

		//todo:fix me
		set_editable_text(in_initial_text.Get(), true);
	}
	FSlateEditableTextLayout::~FSlateEditableTextLayout()
	{
	}
	void FSlateEditableTextLayout::set_text_style(const FTextBlockStyle& in_text_style)
	{
		m_text_style = in_text_style;
	}
	const FTextBlockStyle& FSlateEditableTextLayout::get_text_style() const
	{
		return m_text_style;
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

		case u8'\u000A': //NewLine(Ctrl + Enter), we handle adding new lines via handle carriage return rather than processing new line characters
			return FReply::handled();

		case 1: //swallow ctrl + a, we handle that through on key down
		case 3: //swallow ctrl + c, we handle that through on key down
		case 13: //swallow enter, we handle that through on key down
		case 22: //swallow ctrl + v
		case 24: //swallow ctrl + x, we handle that through on key down
		case 25: //swallow ctrl + y, we handle that through on key down
		case 26: //swallow ctrl + z, we handle that through on key down
		case 27: //swallow esc, we handle that through on key down
		case 127: //swallow ctrl + backspace, we handle that through on key down
			return FReply::handled();
		default:
			//type the character, but only if it is allowed
			return bool_to_reply(handle_type_char(character));
		}

		return FReply::un_handled();
	}
	bool FSlateEditableTextLayout::handle_type_char(const DoDoUtf8String& in_char)
	{
		//todo:delete string

		//certain characters are not allowed

		//todo:check allowed

		const FTextLocation cursor_interaction_position = m_cursor_info.get_cursor_interaction_location();
		const std::vector<FTextLayout::FLineModel>& lines = m_text_layout->get_line_models();
		const FTextLayout::FLineModel& line = lines[cursor_interaction_position.get_line_index()];

		//insert character at caret position
		m_text_layout->insert_at(cursor_interaction_position, in_char);

		//advane caret position

		const FTextLocation final_cursor_location = FTextLocation(cursor_interaction_position.get_line_index(), std::min(cursor_interaction_position.get_offset() + 1, (int32_t)(line.m_text->get_length())));

		m_cursor_info.set_cursor_location_and_calculate_alignment(*m_text_layout, final_cursor_location);
		return true;
	}
	int32_t FSlateEditableTextLayout::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled)
	{
		m_cached_size = glm::vec2(allotted_geometry.get_local_size());

		//only paint the hint text layout if we don't have any text set

		layer_id = m_text_layout->On_Paint(args, allotted_geometry, my_culling_rect, out_draw_elements, layer_id, in_widget_style, b_parent_enabled);

		return layer_id;
	}
	glm::vec2 FSlateEditableTextLayout::Compute_Desired_Size(float layout_scale_multiplier) const
	{
		const glm::vec2 text_layout_size = m_text_layout->get_size();

		return text_layout_size;//todo:fix me
	}
	bool FSlateEditableTextLayout::set_editable_text(const DoDoUtf8String& text_to_set, const bool b_force)
	{
		m_text_layout->clear_lines();

		m_marshaller->set_text(text_to_set, *m_text_layout);

		return true;
	}
	void FSlateEditableTextLayout::cache_desired_size(float layout_scale_multiplier)
	{
		//todo:add more logic

		m_text_layout->update_if_needed();
	}
	FChildren* FSlateEditableTextLayout::Get_Children()
	{
		return m_text_layout->Get_Children();
	}
}