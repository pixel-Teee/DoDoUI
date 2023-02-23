#include <PreCompileHeader.h>

#include "FontMeasure.h"

namespace DoDo
{
	std::shared_ptr<FSlateFontMeasure> FSlateFontMeasure::Create(const std::shared_ptr<FSlateFontCache>& font_cache)
	{
		return std::make_shared<FSlateFontMeasure>(font_cache);
	}

	glm::vec2 FSlateFontMeasure::measure(const DoDoUtf8String& text, const FSlateFontInfo& in_font_info,
		float font_scale) const
	{
		int32_t dummy_last_character_index;
		return measure_string_internal(text, 0, text.get_length(), in_font_info, false, font_scale, -1,
			ELastCharacterIndexFormat::Unused, dummy_last_character_index);
	}

	FSlateFontMeasure::FSlateFontMeasure(const std::shared_ptr<FSlateFontCache>& in_font_cache)
		: m_font_cache(in_font_cache)
	{

	}

	glm::vec2 FSlateFontMeasure::measure_string_internal(const DoDoUtf8String& text, int32_t start_index,
		int32_t end_index, const FSlateFontInfo& in_font_info, bool include_kerning_with_preceding_char,
		float font_scale, int32_t stop_after_horizontal_offset, ELastCharacterIndexFormat char_index_format,
		int32_t out_last_character_index) const
	{
		FCharacterList& character_list = m_font_cache->get_character_list(in_font_info, font_scale);//construct a FSlateFontKey, to find FCharacterList

		const uint16_t max_height = character_list.get_max_height();//FCharacterList will via the FSlateFontCache to get the FSlateFontRenderer, this is a bridge between slate font and free type

		const bool does_start_at_beginning = start_index == 0;
		const bool does_finish_at_end = end_index == text.get_length();
		const int32_t text_range_length = end_index - start_index;

		if(text == "" || end_index - start_index <= 0 || end_index <= 0 || start_index < 0 || end_index <= start_index)
		{
			return glm::vec2(0, max_height);
		}

		//todo:implement FMeasureCache

		//the size of the string
		glm::vec2 size(0.0f);

		//widest line encountered while drawing this text
		int32_t max_line_width = 0;

		//the width of the current line so far
		int32_t current_x = 0;

		//accumulated height for this block of text
		int32_t string_sizey = max_height;

		//character from last frame
		char previous_char = 0;//todo:need to fix this, because our utf8 string

		if(!does_start_at_beginning && include_kerning_with_preceding_char)
		{
			previous_char = text[start_index - 1];
		}

		int32_t final_pos_x = 0;

		int32_t char_index;
		for(char_index = start_index; char_index < end_index; ++char_index)
		{
			char current_char = text[char_index];

			const bool is_new_line = (current_char == '\n');

			if(is_new_line)
			{
				//new line meas
				//we accumulate total height
				string_sizey += max_height;

				//update the longest line we've encountered
				max_line_width = std::max(current_x, max_line_width);

				//the next line starts at the beginning
				current_x = 0;
			}
			else
			{
				const FCharacterEntry& entry = character_list.get_character(current_char, in_font_info.m_font_fallback);

				int32_t kerning = 0;

				//todo:implement get kerning

				previous_char = current_char;

				const int32_t total_char_spacing =
					kerning + entry.m_horizontal_offset + //width is any kerning plus how much to advance the position when drawing a new character
					entry.m_x_advance;//how far we advance

				current_x += kerning + entry.m_x_advance;

				//were we asked to stop measuring after the specified horizontal offset in pixels?
				if(stop_after_horizontal_offset != -1)
				{
					if(char_index_format == ELastCharacterIndexFormat::CharacterAtOffset)
					{
						//round our test toward the character's center position
						if(stop_after_horizontal_offset < current_x - total_char_spacing / 2)
						{
							//we've reached the stopping point, so bail
							break;
						}
					}
					else if(char_index_format == ELastCharacterIndexFormat::LastWholeCharacterBeforeOffset)
					{
						if(stop_after_horizontal_offset < current_x)
						{
							//last whole character before offset is an inclusive index
							--char_index;
							if(char_index < start_index)
							{
								char_index = -1;
							}

							//we've reached the stopping point, so bail
							break;
						}
					}
				}
			}
		}

		//we just finished a line, so need to update the longest line encountered
		max_line_width = std::max(current_x, max_line_width);

		size.x = (float)max_line_width;
		size.y = (float)string_sizey;

		//todo:add last whole character before offset

		out_last_character_index = char_index;

		return size;
	}
}
