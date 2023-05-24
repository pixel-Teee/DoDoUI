#pragma once
#include "FontCache.h"

namespace DoDo
{
	class FSlateFontMeasure
	{
	public:

		static std::shared_ptr<FSlateFontMeasure> Create(const std::shared_ptr<FSlateFontCache>& font_cache);

		virtual ~FSlateFontMeasure() {}

	public:
		/*
		 * measures the width and height of a passed in text, the height is the maximum height of the largest character in the font/size pair
		 *
		 * @param Text the text to measure
		 * @param InFontInfo information about the font that the text is drawn with
		 * @param FontScale the scale to apply to the font
		 * @return the width and height of the text
		 */
		virtual glm::vec2 measure(const DoDoUtf8String& text, const FSlateFontInfo& in_font_info, float font_scale = 1.0f) const;

		FSlateFontMeasure(const std::shared_ptr<FSlateFontCache>& in_font_cache);

		int16_t get_base_line(const FSlateFontInfo& in_font_info, float font_scale) const;

		uint16_t get_max_character_height(const FSlateFontInfo& in_font_info, float font_scale) const;

	private:
		enum ELastCharacterIndexFormat
		{
			//the last whole character before the horizontal offset
			LastWholeCharacterBeforeOffset,
			//the character directly at the offset,
			CharacterAtOffset,
			//not used
			Unused
		};
		/*
		 * measures a string, optionally stopped after the specified horizontal offset in pixels is reached
		 *
		 * @param Text the string to measure
		 * @param StartIndex the inclusive index to start measuring the string at
		 * @param EndIndex the inclusive index to stop measuring the string at
		 * @param FontName the name of the font the string is drawn with
		 * @param FontSize the size of the font
		 * @param StopAfterHorizontalOffset offset horizontally into the string to stop measuring characters after, in pixels (or INDEX_NONE)
		 * @param CharIndexFormat behaviour to use for StopAfterHorizontalOffset
		 * @param OutCharacterIndex the index of the last character processed (used with StopAfterHorizontalOffset)
		 * @param FontScale the scale to apply when measuring the text
		 *
		 * @return the width and height of the string
		 */
		glm::vec2 measure_string_internal(const DoDoUtf8String& text, int32_t start_index, int32_t end_index, const FSlateFontInfo& in_font_info,
			bool include_kerning_with_preceding_char, float font_scale, int32_t stop_after_horizontal_offset, ELastCharacterIndexFormat char_index_format, int32_t& out_last_character_index) const;

		std::shared_ptr<FSlateFontCache> m_font_cache;
	};
}
