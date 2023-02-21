#pragma once

namespace DoDo
{
	struct FShapedGlyphEntry;
	struct FSlateFontInfo;
	struct FFontOutlineSettings;
	struct FCharacterRenderData;
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
	};
}