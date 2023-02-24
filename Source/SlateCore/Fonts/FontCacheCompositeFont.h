#pragma once
#include <unordered_map>

#include "CompositeFont.h"//FFontData record the filename

namespace DoDo
{
	class FFreeTypeLibrary;
	class DoDoUtf8String;

	struct FFontData;
	struct FCompositeFont;
	struct FSlateFontInfo;
	struct FTypeface;

	/*
	 * cached data for a given typeface
	 */
	class FCachedTypefaceData
	{
	public:
		FCachedTypefaceData();

		/*construct the cache from the given typeface*/
		FCachedTypefaceData(const FTypeface& in_type_face, const float in_scaling_factor = 1.0f);

		/*find the font associated with the given name*/
		const FFontData* get_font_data(const DoDoUtf8String& in_name) const;

		/*get the scaling factor for this typeface*/
		float get_scaling_factor() const
		{
			return m_scaling_factor;
		}
	private:
		/*typeface we cached data from*/
		const FTypeface* m_type_face;

		/*array of font data - this is sorted by name for a binary search*/
		//todo:implement FCachedFontData

		/*scaling factor to apply to this typeface*/
		float m_scaling_factor;
	};

	/*
	 * cached data for a given composite font
	 */
	class FCachedCompositeFontData
	{
	public:
		/*default constructor*/
		FCachedCompositeFontData();

		/*construct the cache from the given composite font*/
		FCachedCompositeFontData(const FCompositeFont& in_composite_font);

		/*get the typeface that should be used for the given codepoint*/
		const FCachedTypefaceData* get_type_face_for_code_point(const uint32_t in_code_point) const;

		/*get the default typeface for this composite font*/
		const FCachedTypefaceData* get_default_type_face() const
		{
			return m_cached_type_faces[0].get();
		}
	private:

		/*entry containing a range and the typeface associated with that range*/
		struct FCachedFontRange
		{
			/*range to use for the typeface*/

			/*typeface to which the range applies*/
			std::shared_ptr<FCachedTypefaceData> m_cached_typeface_data;
		};

		/*composite font we cached data from*/
		const FCompositeFont* m_composite_font;

		/*array of cached typefaces - 0 is the default typeface, 1 is the fallback typeface, and the remaining entries are sub-typefaces*/
		std::vector<std::shared_ptr<FCachedTypefaceData>> m_cached_type_faces;
	};

	class FFreeTypeFace;
}

template<>
struct std::hash<DoDo::FFontData>
{
	std::size_t operator()(const DoDo::FFontData& key) const
	{
		return Get_Type_Hash(key);
	}
};

namespace DoDo
{
	/*
 * high-level caching of composite fonts and FreeType font faces
 */
	class FCompositeFontCache
	{
	public:
		/*constructor*/
		FCompositeFontCache(const FFreeTypeLibrary* in_ft_library);

		~FCompositeFontCache();

		/*get the font data to use for the given font info and codepoint*/
		const FFontData& get_font_data_for_code_point(const FSlateFontInfo& in_font_info, const uint32_t in_code_point, float& out_scaling_factor);

		/*gets or loads a freetype font face*/
		//note:this function is important, will load the font file
		std::shared_ptr<FFreeTypeFace> get_font_face(const FFontData& in_font_data);

		/*
		 * get the typeface that should be used for the given codepoint
		 */
		const FCachedTypefaceData* get_cached_type_face_for_code_point(const FCompositeFont* const in_composite_font, const uint32_t in_code_point)
		{
			const FCachedCompositeFontData* const cached_composite_font = get_cached_composite_font(in_composite_font);
			return (cached_composite_font) ? cached_composite_font->get_type_face_for_code_point(in_code_point) : nullptr;
		}
	private:
		/*get the cached composite font data for the given composite font*/
		const FCachedCompositeFontData* get_cached_composite_font(const FCompositeFont* const in_composite_font);

		/*get the default typeface for the given composite font*/
		const FCachedTypefaceData* get_default_cached_type_face(const FCompositeFont* const in_composite_font)
		{
			const FCachedCompositeFontData* const cached_composite_font = get_cached_composite_font(in_composite_font);
			return cached_composite_font ? cached_composite_font->get_default_type_face() : nullptr;
		}

		/*freetype library instance to use*/
		const FFreeTypeLibrary* m_ft_library;

		/*mapping of composite fonts to their cached lookup data*/
		std::map<const FCompositeFont*, std::shared_ptr<FCachedCompositeFontData>> m_composite_font_to_cached_data_map;

		/*mapping of font data to freetype faces*/
		std::unordered_map<FFontData, std::shared_ptr<FFreeTypeFace>> m_font_face_map;
	};
}
