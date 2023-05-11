#include "PreCompileHeader.h"

#include "SlateTextShaper.h"

#include "FontCache.h"//FShapedGlyphEntry depends on it

namespace DoDo {
	FSlateTextShaper::FSlateTextShaper(FFreeTypeCacheDirectory* in_ft_cache_directory, FCompositeFontCache* in_composite_font_cache, FSlateFontRenderer* in_font_renderer, FSlateFontCache* in_font_cache)
		: m_ft_cache_directory(in_ft_cache_directory)
		, m_composite_font_cache(in_composite_font_cache)
		, m_font_renderer(in_font_renderer)
		, m_font_cache(in_font_cache)
		, m_text_bidi_detection(TextBiDi::create_text_bidi())//icu
	{
	}
	FShapedGlyphSequencePtr FSlateTextShaper::shape_bidirectional_text(const DoDoUtf8String& in_text, const int32_t in_text_start, const int32_t in_text_len, const FSlateFontInfo& in_font_info, const float in_font_scale, const TextBiDi::ETextDirection in_base_direction, const ETextShapingMethod text_shaping_method) const
	{
		std::vector<TextBiDi::FTextDirectionInfo> text_direction_infos;

		//note:output the FTextDirectionInfo array
		m_text_bidi_detection->compute_text_direction(in_text, in_text_start, in_text_len, in_base_direction, text_direction_infos);

		std::vector<FShapedGlyphEntry> glyphs_to_render;
		for (const TextBiDi::FTextDirectionInfo& text_direction_info : text_direction_infos)
		{
			perform_text_shaping(in_text, text_direction_info.m_start_index, text_direction_info.m_length, in_font_info, in_font_scale, text_direction_info.m_text_direction,
				text_shaping_method, glyphs_to_render);
		}

		return FShapedGlyphSequencePtr();
	}
	void FSlateTextShaper::perform_text_shaping(const DoDoUtf8String& in_text, const int32_t in_text_start, const int32_t in_text_len, const FSlateFontInfo& in_font_info, const float in_font_scale, const TextBiDi::ETextDirection in_text_direction, const ETextShapingMethod text_shaping_method, std::vector<FShapedGlyphEntry>& out_glyphs_to_render) const
	{

	}
}