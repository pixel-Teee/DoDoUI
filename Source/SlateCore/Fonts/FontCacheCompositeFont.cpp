#include <PreCompileHeader.h>

#include "FontCacheCompositeFont.h"

#include "FontCacheFreeType.h"
#include "SlateFontInfo.h"

namespace DoDo
{
	FCachedTypefaceData::FCachedTypefaceData(const FTypeface& in_type_face, const float in_scaling_factor)
		: m_type_face(&in_type_face)
		, m_scaling_factor(in_scaling_factor)
	{
		//add all the entries from the typeface
		for (const FTypefaceEntry& type_face_entry : m_type_face->m_fonts)
		{
			m_cached_font_data.emplace_back(type_face_entry.m_name, &type_face_entry.m_font);
		}

		//sort the entries now so we binary search them later
		std::sort(m_cached_font_data.begin(), m_cached_font_data.end());
	}

	const FFontData* FCachedTypefaceData::get_font_data(const DoDoUtf8String& in_name) const
	{
		FCachedFontData temp(in_name, nullptr);//temp
		auto it = std::lower_bound(m_cached_font_data.begin(), m_cached_font_data.end(), temp, [](const FCachedFontData& lhs, const FCachedFontData& rhs)->bool
		{
			return lhs.m_name < rhs.m_name;
		});

		if (it != m_cached_font_data.end())
		{
			return it->m_font_data;
		}

		return nullptr;//todo:fix me
	}

	const FFontData* FCachedTypefaceData::get_primary_font_data() const
	{
		return m_type_face->m_fonts.size() > 0 ? &m_type_face->m_fonts[0].m_font : nullptr;
	}

	FCachedCompositeFontData::FCachedCompositeFontData()
		: m_composite_font(nullptr)
	{
	}

	FCachedCompositeFontData::FCachedCompositeFontData(const FCompositeFont& in_composite_font)
		: m_composite_font(&in_composite_font)
	{
		//todo:add default cached type face data

		//add all the entries from the composite font
		m_cached_type_faces.push_back(std::make_shared<FCachedTypefaceData>(m_composite_font->m_default_type_face));

		//todo:implement refresh font ranges
		refresh_font_ranges();
	}

	const FCachedTypefaceData* FCachedCompositeFontData::get_type_face_for_code_point(const uint32_t in_code_point) const
	{
		const int32_t char_index = static_cast<int32_t>(in_code_point);

		auto get_type_face_from_range = [char_index](const std::vector<FCachedFontRange>& in_font_ranges)->const FCachedTypefaceData*
		{
			//note:pass range index, range array index
			auto get_type_face_from_range_index = [char_index, &in_font_ranges](const int32_t in_range_index)->const FCachedTypefaceData*
			{
				if (in_range_index >= 0 && in_range_index < in_font_ranges.size() && in_font_ranges[in_range_index].m_range.Contains(char_index))
				{
					return in_font_ranges[in_range_index].m_cached_typeface_data.get();
				}

				//todo:implement check range
				return nullptr;
			};

			//todo:search InFontRanges
			const int32_t found_range_index = 0;

			if (const FCachedTypefaceData* range_type_face = get_type_face_from_range_index(found_range_index))
			{
				return range_type_face;
			}

			if (const FCachedTypefaceData* range_type_face = get_type_face_from_range_index(found_range_index - 1))
			{
				return range_type_face;
			}
			return nullptr;
		};


		//todo:implement this function
		if (const FCachedTypefaceData* range_type_face = get_type_face_from_range(m_cached_priority_font_ranges))
		{
			return range_type_face;
		}

		return m_cached_type_faces[0].get();
	}

	void FCachedCompositeFontData::refresh_font_ranges()
	{

	}

	FCompositeFontCache::FCompositeFontCache(const FFreeTypeLibrary* in_ft_library)
		: m_ft_library(in_ft_library)
	{

	}

	FCompositeFontCache::~FCompositeFontCache()
	{
	}

	const FFontData& FCompositeFontCache::get_default_font_data(const FSlateFontInfo& in_font_info)
	{
		static const FFontData dummy_font_data;//font data just font name and font file name

		const FCompositeFont* const resolve_composite_font = in_font_info.get_composite_font();

		//FCachedTypefaceData have FTypeFace and FCacheFontData(font name + FFontData)
		const FCachedTypefaceData* const cached_type_face_data = get_default_cached_type_face(resolve_composite_font);

		if (cached_type_face_data)
		{
			//try to find the correct font from the typeface
			//typeface is FTypeFaceEntry array, FTypeFaceEntry contains the FFontData
			const FFontData* found_font_data = cached_type_face_data->get_font_data(in_font_info.m_type_face_font_name);
			if (found_font_data)
			{
				return *found_font_data;
			}

			//failing that, return the primary font
			found_font_data = cached_type_face_data->get_primary_font_data();

			if (found_font_data)
			{
				return *found_font_data;
			}
		}

		return dummy_font_data;
	}

	const FFontData& FCompositeFontCache::get_font_data_for_code_point(const FSlateFontInfo& in_font_info,
		const uint32_t in_code_point, float& out_scaling_factor)
	{
		static const FFontData dummy_font_data;

		auto get_font_data_for_character_in_typeface = [this, in_code_point, &in_font_info](const FCachedTypefaceData* in_cached_type_face_data,
			const FCachedTypefaceData* in_cached_default_type_face_data, const bool in_skip_character_check) -> const FFontData*
		{
			//try to find the correct font from the typeface
			const FFontData* found_font_data = in_cached_type_face_data->get_font_data(in_font_info.m_type_face_font_name);

			return found_font_data;//todo:add check
		};

		const FCompositeFont* const resolved_composite_font = in_font_info.get_composite_font();//note:from FSlateFontInfo to get the FCompositeFont
		const FCachedTypefaceData* const cached_type_face_data = get_cached_type_face_for_code_point(resolved_composite_font, in_code_point);

		if(cached_type_face_data)
		{
			const FCachedTypefaceData* const cached_default_type_face_data = get_default_cached_type_face(resolved_composite_font);

			//pass two FCachedTypefaceData
			//check the preferred typeface first
			if(const FFontData* found_font_data = get_font_data_for_character_in_typeface(cached_type_face_data, cached_default_type_face_data, false))
			{
				out_scaling_factor = cached_type_face_data->get_scaling_factor();
				return *found_font_data;
			}
		}

		out_scaling_factor = 1.0f;
		return dummy_font_data;
	}

	std::shared_ptr<FFreeTypeFace> FCompositeFontCache::get_font_face(const FFontData& in_font_data)
	{
		auto it = m_font_face_map.find(in_font_data);
		std::shared_ptr<FFreeTypeFace> face_and_memory;
		if (it != m_font_face_map.end())
			face_and_memory = it->second;

		if(!face_and_memory)
		{
			//load
			//FFontFaceDataConstPtr font_face_data = in_font_data.get_font_face_data();
			face_and_memory = std::make_shared<FFreeTypeFace>(m_ft_library, in_font_data.get_font_file_name(), in_font_data.get_sub_face_index());

			//got a valid font?
			if(face_and_memory)
			{
				m_font_face_map.insert({ in_font_data, face_and_memory });
			}
		}

		return face_and_memory;
	}

	const FCachedCompositeFontData* FCompositeFontCache::get_cached_composite_font(
		const FCompositeFont* const in_composite_font)
	{
		if(!in_composite_font)
		{
			return nullptr;
		}

		auto it = m_composite_font_to_cached_data_map.find(in_composite_font);

		if(it != m_composite_font_to_cached_data_map.end())
		{
			return it->second.get();
		}

		//note:use FCompositeFont to construct a FCachedCompositeFontData
		auto it2 = m_composite_font_to_cached_data_map.insert({ in_composite_font, std::make_shared<FCachedCompositeFontData>(*in_composite_font) });
		return it2.first->second.get();//note:return FCachedCompositeFontData
	}
}
