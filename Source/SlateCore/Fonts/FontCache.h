#pragma once
#include "FontTypes.h"//FSlateFontKey depends on it

#include "SlateFontInfo.h"

#include "SlateTextShaper.h"//FShapedGlyphFaceData depends on it

#include "SlateCore/Textures/TextureAtlas.h"//ISlateAtlasProvider depends on it

namespace DoDo
{
	/*the font atlas data for a single glyph in a shaped text sequence*/
	struct FShapedGlyphFontAtlasData
	{
		/*the vertical distance from the baseline to the topmost border of the glyph bitmap*/
		int16_t m_vertical_offset = 0;

		/*the horizontal distance from the origin to the leftmost border of the glyph bitmap*/
		int16_t m_horizontal_offset = 0;

		/*start x location of the glyph in the texture*/
		uint16_t m_start_u = 0;

		/*start y location of the glyph in the texture*/
		uint16_t m_start_v = 0;

		/*index to a specific texture in the font cache*/
		uint8_t m_texture_index = 0;

		/*true if this entry supports outline rendering, false otherwise*/
		bool m_supports_outline = false;

		/*true if this entry is valid, false otherwise*/
		bool m_valid = false;
	};

	/*information for rendering one glyph in a shaped text sequence*/
	struct FShapedGlyphEntry//note:one glyph
	{
		friend class FSlateFontCache;

		/*provides access to the FreeType face for this glyph (not available publicly)*/
		std::shared_ptr<FShapedGlyphFaceData> m_font_face_data;

		/*the index of this glyph in the FreeType face*/
		uint32_t m_graph_index = 0;

		/*the index of this glyph from the source text, the source indices may skip characters if the sequence contains ligatures, additionally, some characters produce multiple glyphs leading to duplicate source indices*/
		int32_t m_source_index = 0;

		/*the amount to advance in x before drawing the next glyph in the sequence*/
		int16_t m_x_advance = 0;

		/*the amount to advance in y before drawing the next glyph in the sequence*/
		int16_t m_y_advance = 0;

		/*the offset to apply in x when drawing this glyph*/
		int16_t m_x_offset = 0;

		/*the offset to apply in y when drawing this glyph*/
		int16_t m_y_offset = 0;

		/*
		 * the "kerning" between this glyph and the next one in the sequence
		 *
		 * @note this value is included in the XAdvance so you never usually need it unless you're manually combining two sets of glyphs together
		 * @note this value isn't strictly the kerning value - it's simply the difference between the glyphs horizontal advance, and the shaped horizontal advance (so will contain any accumulated advance added by the shaper)
		 */
		int8_t m_kerning = 0;

		/*
		 * the number of source characters represented by this glyph
		 * this is typically 1, however will be greater for ligatures, or may be 0 if a single character produces multiple glyphs
		 */
		uint8_t m_num_characters_in_glyph = 0;

		/*
		 * the number of source grapheme clusters represented by this glyph
		 * this is typically 1, however will be greater for ligatures, or may be 0 if a single character produces multiple glyphs
		 */
		uint8_t m_num_grapheme_clusters_in_glyph = 0;

		/*
		 * the reading direction of the text this glyph was shaped from
		 */
		//todo:add TextBiDi::ETextDirection

		/*
		 * true if this is a visible glyph that should be drawn
		 * false if the glyph is invisible (eg, whitespace or a control code) and should skip drawing, but still include its advance amount
		 */
		bool m_b_is_visible = false;

		/*check whether this entry contains a valid glyph (non-zero, and not the SlateFontRendererUtils::InvalidSubChar glyph)*/
		bool has_valid_glyph() const;

		/*get any additional scale that should be applied when rendering this glyph*/
		float get_bit_map_render_scale() const;

	private:
		/*
		 * pointer to the cached atlas data for this glyph entry
		 * this is cached on the glyph by FSlateFontCache::GetShapedGlyphFontAtlasData to avoid repeated map look ups
		 *
		 * first index is to determine if this is a cached outline glyph or a regular glyph
		 *
		 * second index is the index of the thread dependent font cache, index 0 is the cache value for the game thread font cache, index 1 is the cached value for the render thread font cache
		 */
		mutable std::weak_ptr<FShapedGlyphFontAtlasData> m_cached_atlas_data[2][2];//EFontCacheAtlasDataType::Num
	};

	/*
	 * manages a potentially large list of non-shaped characters
	 * uses a directly indexed by TCHAR array until space runs out and maps the rest to conserve memory
	 * every character indexed by TCHAR could potentially cost a lot of memory of a lot of empty entries are created
	 * because characters being are far apart
	 */
	class FCharacterList
	{
	public:
		FCharacterList(const FSlateFontKey& in_font_key, FSlateFontCache& in_font_cache);

	private:
		/*Maintains a fake shaped glyph for each character in the character list*/
		struct FCharacterListEntry
		{
			/*the shaped glyph data for this character*/
			FShapedGlyphEntry m_shaped_glyph_entry;
			/*font data this character was rendered with*/
			const FFontData* m_font_data = nullptr;

			/*does this character have kerning?*/
			bool m_has_kerning = false;

			/*has this entry been initialized?*/
			bool m_valid = false;
		};
		/*entries for larger character sets to conserve memory*/
		std::map<DoDoUtf8String, FCharacterListEntry> m_mapped_entries;
		/*font for this character list*/
		FSlateFontKey m_font_key;

		/*reference to the font cache for accessing new unseen characters*/
		FSlateFontCache& m_font_cache;
	};
	/*
	 * font caching implementation
	 *
	 * caches characters into texture as needed
	 */
	class FSlateFontCache : public ISlateAtlasProvider
	{
	public:
		/*
		 * constructor
		 *
		 * @param InTextureSize the size of the atlas texture
		 * @param InFontAtlas platform specific font atlas resource
		 */
		//todo:implement ISlateFontAtlasFactory
		FSlateFontCache(std::shared_ptr<ISlateFontAtlasFactory> in_font_atlas_factory);

		/*
		 * gets information for how to draw all non-shaped characters in the specified string, caches characters as they are found
		 *
		 * @param InFontInfo information about the font that the string is drawn with
		 * @param FontScale the scale to apply to the font
		 * @param OutCharacterEntries populated array of character entries, indices of characters in text match indices in this array
		 */
		FCharacterList& get_character_list(const FSlateFontInfo& in_font_info, float font_scale, const FFontOutlineSettings& in_outline_settings = FFontOutlineSettings::NoOutline);


		virtual int32_t get_num_atlas_pages() const override;

		virtual FSlateShaderResource* get_atlas_page_resource(const int32_t in_index) override;

		virtual bool is_atlas_page_resource_alpha_only(const int32_t in_index) const override;
	private:

		/*mapping font keys to cached data*/
		std::unordered_map<FSlateFontKey, std::unique_ptr<FCharacterList>> m_font_to_character_list_cache;

		/*factory for creating new font atlases*/
		std::shared_ptr<ISlateFontAtlasFactory> m_font_atlas_factory;
		
	};
}
