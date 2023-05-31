#pragma once

#include "Slate/Framework/Text/TextLayout.h"//FTextLocation depends on it

namespace DoDo {
	namespace SlateEditableTextTypes
	{
		enum class ECursorAlignment : uint8_t
		{
			/*visually align the cursor to the left of the character its placed at, and insert text before the character*/
			Left,

			/*visually align the cursor to the right of the character its placed at, and insert text after the character*/
			Right
		};

		/*store the information about the current cursor position*/
		class FCursorInfo
		{
		public:
			FCursorInfo()
				: m_cursor_position()
				, m_cursor_alignment(ECursorAlignment::Left)
				, m_cursor_text_direction(TextBiDi::ETextDirection::LeftToRight)
				, m_last_cursor_interation_time(0)
			{}

			/*get the literal position of the cursor (note:this may not be the correct place to insert text to, use get cursor interaction location for that)*/
			FTextLocation get_cursor_location() const
			{
				return m_cursor_position;
			}

			/*get the interaction position of the cursor (where to insert, delete, etc, text from/to)*/
			FTextLocation get_cursor_interaction_location() const
			{
				//if the cursor is right aligned, we treat it as if its were one character further along for insert/delete purposes
				return FTextLocation(m_cursor_position, (m_cursor_alignment == ECursorAlignment::Right) ? 1 : 0);
			}

			/*set the position of the cursor, and then work out the correct alignment based on the current text layout*/
			void set_cursor_location_and_calculate_alignment(const FTextLayout& in_text_layout, const FTextLocation& in_cursor_position);

			/*set the literal position and alignment of the cursor*/
			void set_cursor_location_and_alignment(const FTextLayout& in_text_layout, const FTextLocation& in_cursor_position, const ECursorAlignment in_cursor_alignment);
		private:
			/*current cursor position, there is always one*/
			FTextLocation m_cursor_position;

			/*cursor alignment (horizontal) within its position, this affects the rendering and behaviour of the cursor when adding new characters*/
			ECursorAlignment m_cursor_alignment;

			/*the directional of the text under the cursor*/
			TextBiDi::ETextDirection m_cursor_text_direction;

			/*last time the user did anything with the cursor*/
			double m_last_cursor_interation_time;
		};
	}
}