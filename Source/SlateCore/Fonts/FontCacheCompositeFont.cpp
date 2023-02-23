#include <PreCompileHeader.h>

#include "FontCacheCompositeFont.h"

#include "FontCacheFreeType.h"
#include "SlateFontInfo.h"

namespace DoDo
{
	const FFontData* FCachedTypefaceData::get_font_data(const DoDoUtf8String& in_name) const
	{

	}

	FCachedCompositeFontData::FCachedCompositeFontData()
		: m_composite_font(nullptr)
	{
	}

	FCachedCompositeFontData::FCachedCompositeFontData(const FCompositeFont& in_composite_font)
		: m_composite_font(&in_composite_font)
	{
		//todo:add default cached type face data
	}

	const FCachedTypefaceData* FCachedCompositeFontData::get_type_face_for_code_point(const uint32_t in_code_point) const
	{
		const int32_t char_index = static_cast<int32_t>(in_code_point);

		auto get_type_face_from_range = [char_index](const std::vector<FCachedFontRange>& in_font_ranges)->const FCachedTypefaceData*
		{
			auto get_type_face_from_range_index = [char_index, &in_font_ranges](const int32_t in_range_index)
			{
				//todo:implement check range
				return in_font_ranges[in_range_index].m_cached_typeface_data.get();
			};

			


			return nullptr;
		};


		//todo:implement this function

		return nullptr;
	}

	FCompositeFontCache::~FCompositeFontCache()
	{
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
		};

		const FCompositeFont* const resolved_composite_font = in_font_info.get_composite_font();//note:from FSlateFontInfo to get the FCompositeFont
		const FCachedTypefaceData* const cached_type_face_data = get_cached_type_face_for_code_point(resolved_composite_font, in_code_point);

		if(cached_type_face_data)
		{
			const FCachedTypefaceData* const cached_default_type_face_data = get_default_cached_type_face(resolved_composite_font);

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

			face_and_memory = std::make_shared<FFreeTypeFace>(m_ft_library, in_font_data.get_font_file_name(), in_font_data.get_sub_face_index());
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
