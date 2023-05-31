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
	FReply FSlateEditableTextLayout::handle_key_down(const FKeyEvent& in_key_event)
	{
		FReply reply = FReply::un_handled();

		const FKey key = in_key_event.get_key();

		if (key == EKeys::Enter) //todo:check is read only
		{
			handle_carriage_return(in_key_event.is_repeat());

			reply = FReply::handled();
		}

		return reply;
	}
	FReply FSlateEditableTextLayout::handle_mouse_button_down(const FGeometry& my_geometry, const FPointerEvent& in_mouse_event)
	{
		FReply reply = FReply::un_handled();

		//if the mouse is already captured, then don't allow a new action to be taken
		if (!m_owner_widget->get_slate_widget()->has_mouse_capture())
		{
			if (in_mouse_event.get_effecting_button() == EKeys::LeftMouseButton ||
				in_mouse_event.get_effecting_button() == EKeys::RightMouseButton)
			{
				//am i getting foucs right now?
				const bool b_is_getting_focus = !m_owner_widget->get_slate_widget()->has_any_user_focus().has_value();

				if (b_is_getting_focus)
				{

				}
				else
				{
					//todo:show virtual key board interms of platform
				}

				if (in_mouse_event.get_effecting_button() == EKeys::LeftMouseButton)
				{
					
				}
				else if (in_mouse_event.get_effecting_button() == EKeys::RightMouseButton)
				{

				}

				//right clicking to summon context menu, but we'll do that on mouse-up
				reply = FReply::handled();
				reply.capture_mouse(m_owner_widget->get_slate_widget());
				reply.set_user_focus(m_owner_widget->get_slate_widget(), EFocusCause::Mouse);
			}
		}

		return reply;
	}
	FReply FSlateEditableTextLayout::handle_mouse_button_up(const FGeometry& my_geometry, const FPointerEvent& in_mouse_event)
	{
		FReply reply = FReply::un_handled();

		//the mouse must have been captured by either left or right button down before we'll process mouse ups
		if (m_owner_widget->get_slate_widget()->has_mouse_capture())
		{
			if (in_mouse_event.get_effecting_button() == EKeys::LeftMouseButton)
			{
				//release mouse capture
				reply = FReply::handled();
				reply.release_mouse_capture();
			}
			else if (in_mouse_event.get_effecting_button() == EKeys::RightMouseButton)
			{
				//release mouse capture
				reply = FReply::handled();
				reply.release_mouse_capture();
			}
		}

		return reply;
	}
	bool FSlateEditableTextLayout::handle_carriage_return(bool is_repeat)
	{
		//todo:check read only

		if (is_repeat)
		{
			//skip committing text on a repeat enter key
			return false;
		}
		else
		{
			const DoDoUtf8String edited_text = get_editable_text();

			//when enter is pressed text is committed, let anyone interested know about it
			m_owner_widget->on_text_committed(edited_text, ETextCommit::OnEnter);
		}

		return true;
	}
	void FSlateEditableTextLayout::update_cursor_highlight()
	{
		const bool b_has_keyboard_focus = m_owner_widget->get_slate_widget()->has_any_user_focus().has_value();

		const bool b_is_read_only = m_owner_widget->is_text_read_only();

		//todo:add more logic

		if (b_has_keyboard_focus && !b_is_read_only)
		{
			//the cursor mode uses the literal position rather than the interaction position
			const FTextLocation cursor_position = m_cursor_info.get_cursor_location();

			const std::vector<FTextLayout::FLineModel>& lines = m_text_layout->get_line_models();

			if (cursor_position.get_line_index() >= 0 && cursor_position.get_line_index() < lines.size())
			{
				//ensure the cursor is sitting on a valid graph me
				//const FTextLocation cursor_selection = m_text_layout->get_graph_me_at(cursor_position);

			}
		}
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
	DoDoUtf8String FSlateEditableTextLayout::get_editable_text() const
	{
		DoDoUtf8String edited_text;
		m_marshaller->get_text(edited_text, *m_text_layout);

		return edited_text;
	}
	void FSlateEditableTextLayout::cache_desired_size(float layout_scale_multiplier)
	{
		//todo:add more logic
		m_text_layout->set_scale(layout_scale_multiplier);
		m_text_layout->update_if_needed();
	}
	FChildren* FSlateEditableTextLayout::Get_Children()
	{
		return m_text_layout->Get_Children();
	}
}