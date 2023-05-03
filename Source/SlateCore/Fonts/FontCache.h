#pragma once
#include "FontTypes.h"//FSlateFontKey depends on it

#include "SlateFontInfo.h"

#include "SlateTextShaper.h"//FShapedGlyphFaceData depends on it

#include "SlateCore/Textures/TextureAtlas.h"//ISlateAtlasProvider depends on it

#include "SlateFontRenderer.h"//FSlateFontCache depends on it
#include "FontCacheCompositeFont.h"//FCompositeFontCache depends on it

namespace DoDo
{
	enum class EFontCacheAtlasDataType : uint8_t
	{
		/*data was cached for a regular non-outline font*/
		Regular = 0,

		/*data was cached for a outline (stroked) font*/
		Outline,

		/*must be last*/
		Num
	};

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

		/*x size of the glyph in the texture*/
		uint16_t m_u_size = 0;

		/*y size of the glyph in the texture*/
		uint16_t m_v_size = 0;

		/*index to a specific texture in the font cache*/
		uint8_t m_texture_index = 0;

		/*true if this entry supports outline rendering, false otherwise*/
		bool m_supports_outline = false;

		/*true if this entry is valid, false otherwise*/
		bool m_valid = false;
	};

	class FFreeTypeFace;
	/*minimal FShapedGlyphEntry key information used for map lookups*/
	struct FShapedGlyphEntryKey
	{
	public:
		FShapedGlyphEntryKey(const FShapedGlyphFaceData& in_font_face_data, uint32_t in_glyph_index, const FFontOutlineSettings& in_outline_settings);

		friend inline uint32_t Get_Type_Hash(const FShapedGlyphEntryKey& key)
		{
			return key.m_key_hash;
		}

		bool operator==(const FShapedGlyphEntryKey& other) const
		{
			return m_font_face.lock() == other.m_font_face.lock()
				&& m_font_size == other.m_font_size
				&& m_outline_size == other.m_outline_size
				&& m_outline_separate_fill_alpha == other.m_outline_separate_fill_alpha
				&& m_font_scale == other.m_font_scale
				&& m_glyph_index == other.m_glyph_index;
		}
	private:
		/*weak pointer to the freetype face to render with*/
		std::weak_ptr<FFreeTypeFace> m_font_face;
		/*provides the point size used to render the font*/
		int32_t m_font_size;
		/*the size in pixels of the outline to render for the font*/
		float m_outline_size;
		/*if checked, the outline will be completely translucent where the filled area will be*/
		bool m_outline_separate_fill_alpha;
		/*provides the final scale used to render to the font*/
		float m_font_scale;
		/*the index of this glyph in the free type face*/
		uint32_t m_glyph_index;
		/*cached has value used for map lookups*/
		uint32_t m_key_hash;
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

	/*information for rendering one non-shaped character*/
	struct FCharacterEntry
	{
		/*the character this entry is for*/
		DoDoUtf8String character = 0;//todo:need to fix this
		/*the index of the glyph from the FreeType face that this entry is for*/
		uint32_t m_glyph_index = 0;
		/*the raw font data this character was rendered with*/
		const FFontData* m_font_data = nullptr;
		/*the kerning cache that this entry uses*/
		//std::shared_ptr<FFreeTypeKerningCache>
		/*scale that was applied when rendering this character*/
		float m_font_scale = 0.0f;
		/*any additional scale that should be applied when rendering this glyph*/
		float m_bitmap_render_scale = 0.0f;
		/*start x location of the character in the texture*/
		uint16_t m_start_u = 0;
		/*start y location of the character in the texture*/
		uint16_t m_start_v = 0;
		/*x size of the character in the texture*/
		uint16_t m_u_size = 0;
		/*y size of the character in the texture*/
		uint16_t m_v_size = 0;
		/*the vertical distance from the baseline to the topmost border of the character*/
		int16_t m_vertical_offset = 0;
		/*the vertical distance from the baseline to the left most border of the character*/
		int16_t m_horizontal_offset = 0;
		/*the largest vertical distance below the baseline for any character in the font*/
		int16_t m_global_descendar = 0;
		/*the amount to advance in x before drawing the next character in a string*/
		int16_t m_x_advance = 0;
		/*index to a specific texture in the font cache*/
		uint8_t m_texture_index = 0;
		/*the fallback level this character represents*/
		EFontFallback m_fall_back_level = EFontFallback::FF_Max;
		/*1 if this entry has kerning, 0 otherwise*/
		bool m_has_kerning = false;
		/*1 if this entry supports outline rendering, 0 otherwise*/
		bool m_supports_outline = false;
		/*1 if this entry is valid, 0 otherwise*/
		bool m_valid = false;
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

		/*
		 * gets data about how to render and measure a character
		 * caching and atlasing it if needed
		 *
		 * @param Character the character to get
		 * @param MaxFontFallback the maximum fallback level that can be used when resolving glyphs
		 * @return data about the character
		 */
		FCharacterEntry get_character(DoDoUtf8String character, const EFontFallback max_font_fall_back);

		/*
		 * @return the global max height for any character in this font
		 */
		uint16_t get_max_height() const;

	private:
		/*
		* returns whether the specified character is valid for caching (i.e. whether it matches the font fall back level)
		* 
		* @param character the character to check
		* @param MaxFontFllback the maximum fallback level that can be used when resolving glyphs
		*/
		bool can_cache_character(DoDoUtf8String character, const EFontFallback max_font_fall_back) const;

		/*Maintains a fake shaped glyph for each character in the character list*/
		struct FCharacterListEntry
		{
			/*the shaped glyph data for this character*/
			FShapedGlyphEntry m_shaped_glyph_entry;
			/*font data this character was rendered with*/
			const FFontData* m_font_data = nullptr;

			/*the fallback level this character uses*/
			EFontFallback m_fall_back_level = EFontFallback::FF_Max;

			/*does this character have kerning?*/
			bool m_has_kerning = false;

			/*has this entry been initialized?*/
			bool m_valid = false;
		};

		/*
		 * caches a new character
		 *
		 * @param Character the character to cache
		 */
		FCharacterListEntry* cache_character(DoDoUtf8String character);

		/*
		* convert the cached internal entry to the external data for the old non-shaped api
		*/
		FCharacterEntry make_character_entry(DoDoUtf8String character, const FCharacterListEntry& internal_entry) const;

		/*entries for larger character sets to conserve memory*/
		std::map<DoDoUtf8String, FCharacterListEntry> m_mapped_entries;

		/*directly indexed entries for fast lookup*/
		std::vector<FCharacterListEntry> m_direct_index_entries;

		/*font for this character list*/
		FSlateFontKey m_font_key;

		/*reference to the font cache for accessing new unseen characters*/
		FSlateFontCache& m_font_cache;

		/*number of directly indexed entries*/
		int32_t m_max_direct_indexed_entries;

		/*the global max height for any character in this font*/
		mutable uint16_t m_max_height;
	};
	
}

template<>
struct std::hash<DoDo::FShapedGlyphEntryKey>
{
	std::size_t operator()(const DoDo::FShapedGlyphEntryKey& key) const
	{
		return Get_Type_Hash(key);
	}
};

namespace DoDo
{
	/*
	 * font caching implementation
	 *
	 * caches characters into texture as needed
	 */
	class FSlateFontCache : public ISlateAtlasProvider
	{
	public:
		friend FCharacterList;
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


		ISlateFontTexture* get_font_texture(uint32_t index) { return m_all_font_textures[index].get(); }

		/*
		* gets the atlas information for the given shaped glyph, this information will be cached if required
		*/
		FShapedGlyphFontAtlasData get_shaped_glyph_font_atlas_data(const FShapedGlyphEntry& in_shaped_glyph, const FFontOutlineSettings& in_outline_settings);

		virtual int32_t get_num_atlas_pages() const override;

		virtual FSlateShaderResource* get_atlas_page_resource(const int32_t in_index) override;

		virtual bool is_atlas_page_resource_alpha_only(const int32_t in_index) const override;

		/*
		* add a new entries into a cache atlas
		*
		* @param InFontInfo information about the font being used for the characters
		* @param Characters the characters to cache
		* @param FontScale the font scale to use
		*
		* @return true if the characters could be cached, false if the cache is full
		*/
		bool add_new_entry(const FShapedGlyphEntry& in_shaped_glyph, const FFontOutlineSettings& in_outline_settings, FShapedGlyphFontAtlasData& out_atlas_data);

		bool add_new_entry(const FCharacterRenderData in_render_data, uint8_t& out_texture_index, uint16_t& out_glyph_x, uint16_t& out_glyph_y, uint16_t& out_glyph_width, uint16_t& out_glyph_height);
	public:
		/*
		 * updates the texture used for rendering
		 */
		void update_cache();//note:this function will call font atlas texture function to update runtime shader texture

		/*
		 * returns the height of the largest character in the font
		 *
		 * @param InFontInfo a descriptor of the font to get character size for
		 * @param FontScale the scale to apply to the font
		 *
		 * @return the largest character height
		 */
		uint16_t get_max_character_height(const FSlateFontInfo& in_font_info, float font_scale) const;
	private:
		/*
		* clears all cached data from the cache
		*/
		bool flush_cache();

		/*FreeType library instance (owned by this font cache)*/
		std::unique_ptr<FFreeTypeLibrary> m_ft_library;

		/*FreeType library cache directory (owned by this font cache)*/
		std::unique_ptr<FFreeTypeCacheDirectory> m_ft_cache_directory;

		/*high-level composite font cache (owned by this font cache)*/
		std::unique_ptr<FCompositeFontCache> m_composite_font_cache;

		/*FreeType font renderer (owned by this font cache)*/
		std::unique_ptr<FSlateFontRenderer> m_font_renderer;

		/*mapping font keys to cached data*/
		std::unordered_map<FSlateFontKey, std::unique_ptr<FCharacterList>> m_font_to_character_list_cache;

		/*factory for creating new font atlases*/
		std::shared_ptr<ISlateFontAtlasFactory> m_font_atlas_factory;

		/*mapping shaped glyphs to their cached atlas data*/
		std::unordered_map<FShapedGlyphEntryKey, std::shared_ptr<FShapedGlyphFontAtlasData>> m_shaped_glyph_to_atlas_data;

		/*array of grayscale font atlas indices for use with all font textures (cast the element to FSlateFontAtlas)*/
		std::vector<uint8_t> m_gray_scale_font_atlas_indices;

		/*array of color font atlas indices for use with all font texture (cast the element to FSlateFontAtlas)*/
		std::vector<uint8_t> m_color_font_atlas_indices;

		/*array of all font textures - both atlased and non-atlased*/
		std::vector<std::shared_ptr<ISlateFontTexture>> m_all_font_textures;
	};
}
