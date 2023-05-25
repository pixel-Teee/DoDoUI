#include "PreCompileHeader.h"

#include "SlateEditableTextTypes.h"

namespace DoDo {
	void SlateEditableTextTypes::FCursorInfo::set_cursor_location_and_calculate_alignment(const FTextLayout& in_text_layout, const FTextLocation& in_cursor_position)
	{
		FTextLocation new_cursor_position = in_cursor_position;
		ECursorAlignment new_alignment = ECursorAlignment::Left;

		{
			const int32_t cursor_line_index = in_cursor_position.get_line_index();
			const int32_t cursor_offset = in_cursor_position.get_offset();

			//a cursor offset of zero could mark the end of an empty line, but we don't need to adjust the cursor fr an empty line
			if (cursor_offset > 0)
			{
				const std::vector<FTextLayout::FLineModel>& lines = in_text_layout.get_line_models();
				const FTextLayout::FLineModel& line = lines[cursor_line_index];
				if (line.m_text->get_length() == cursor_offset)
				{
					//we need to move the cursor back one from where it currently is, this keeps the interaction point the same
					//since the cursor is aligned to the right, but visually draws the cursor in the correct place
					new_cursor_position = FTextLocation(new_cursor_position, -1);
					new_alignment = ECursorAlignment::Right;
				}
			}
		}

		set_cursor_location_and_alignment(in_text_layout, new_cursor_position, new_alignment);
	}
	void SlateEditableTextTypes::FCursorInfo::set_cursor_location_and_alignment(const FTextLayout& in_text_layout, const FTextLocation& in_cursor_position, const ECursorAlignment in_cursor_alignment)
	{
		m_cursor_position = in_cursor_position;
		m_cursor_alignment = in_cursor_alignment;
		m_cursor_text_direction = TextBiDi::ETextDirection::LeftToRight;

		//todo:add more logic
	}
}


