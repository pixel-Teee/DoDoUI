#pragma once

#include "SlateFontInfo.h"//EFontFallback depends on it

namespace DoDo
{
	class FFreeTypeFace;
	/*
	* internal struct for passing around information about loading a glyph
	*/
	struct FFreeTypeFaceGlyphData
	{
		/*the fort face for the character*/
		std::shared_ptr<FFreeTypeFace> m_face_and_memory;

		/*the glyph index for the character*/
		uint32_t m_glyph_index;

		/*the glyph flags that should be used for loading the characters glyph*/
		uint32_t m_glyph_flags;

		/*the fallback font set the character was loaded from*/
		EFontFallback m_char_fall_back_level;

		FFreeTypeFaceGlyphData()
			: m_face_and_memory()
			, m_glyph_index(0)
			, m_glyph_flags(0)
			, m_char_fall_back_level(EFontFallback::FF_NoFallback)
		{}
	};

	struct FShapedGlyphEntry;
	struct FSlateFontInfo;
	struct FFontOutlineSettings;
	struct FCharacterRenderData;
	class FFreeTypeLibrary;
	/*
	 * bridging point between FreeType and the Slate font system
	 * this class, via the instances you pass to its constructor, knows how to correctly render a slate font
	 */
	class FSlateFontRenderer
	{
	public:
		/*
		 * @return the global max height for any character in the default font
		 */
		uint16_t get_max_height(const FSlateFontInfo& in_font_info, const float in_scale) const;

		/*
		* creates render data for a specific character
		* 
		* @param InFontData raw font data to render the character with
		* @param InSize the size of the font to draw
		* @param InOutlineSettings outline settings to apply when rendering the character
		* @param Char the character to render
		* @param OutRenderData will contain the created render data
		* @param InScale the scale of the font
		* @param OutFallbackLevel outputs the fallback level of the font
		*/
		bool get_render_data(const FShapedGlyphEntry& in_shaped_glyph, const FFontOutlineSettings& in_outline_settings, FCharacterRenderData& out_render_data) const;

		//todo:implement FFreeTypeLibrary, FFreeTypeCacheDirectory, FCompositeFontCache

		bool get_render_data_internal(const FFreeTypeFaceGlyphData& in_face_glyph_data, const float in_scale, const FFontOutlineSettings& in_outline_settings, FCharacterRenderData& out_render_data) const;

	private:

		const FFreeTypeLibrary* m_ft_library;
	};
}