#pragma once
#include <unordered_map>

#include "CompositeFont.h"//FFontData record the filename

#include "Core/Math/Range.h"

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

		/*get the font data associated with primary (first) entry in this typeface*/
		const FFontData* get_primary_font_data() const;

		/*get the scaling factor for this typeface*/
		float get_scaling_factor() const
		{
			return m_scaling_factor;
		}
	private:
		/*entry containing a name and the font data associated with that name*/
		struct FCachedFontData
		{
			FCachedFontData()
			: m_name()
			, m_font_data(nullptr)
			{}

			FCachedFontData(const DoDoUtf8String in_name, const FFontData* in_font_data)
				: m_name(in_name)
				, m_font_data(in_font_data)
			{}

			bool operator<(const FCachedFontData& rhs) const {
				return m_name < rhs.m_name;
			}

			/*name of the font*/
			DoDoUtf8String m_name;

			/*data of the font*/
			const FFontData* m_font_data;
		};

		/*typeface we cached data from*/
		const FTypeface* m_type_face;

		/*array of font data - this is sorted by name for a binary search*/
		//todo:implement FCachedFontData
		std::vector<FCachedFontData> m_cached_font_data;

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

		/*refresh the font ranges used by sub-fonts (should be called when the culture is changed)*/
		void refresh_font_ranges();
	private:

		/*entry containing a range and the typeface associated with that range*/
		struct FCachedFontRange
		{
			/*default constructor*/
			FCachedFontRange()
				: m_range(FInt32Range::Empty())
				, m_cached_typeface_data()
			{}

			/*range to use for the typeface*/
			FInt32Range m_range;

			/*typeface to which the range applies*/
			std::shared_ptr<FCachedTypefaceData> m_cached_typeface_data;
		};

		/*composite font we cached data from*/
		const FCompositeFont* m_composite_font;

		/*array of cached typefaces - 0 is the default typeface, 1 is the fallback typeface, and the remaining entries are sub-typefaces*/
		std::vector<std::shared_ptr<FCachedTypefaceData>> m_cached_type_faces;

		/*array of font ranges paired with their associated typefaces - this is sorted in ascending order for a binary search*/
		std::vector<FCachedFontRange> m_cached_priority_font_ranges;
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

		/*get the default font data to use for the given font info*/
		const FFontData& get_default_font_data(const FSlateFontInfo& in_font_info);

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
