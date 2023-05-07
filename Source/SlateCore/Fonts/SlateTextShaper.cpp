#include "PreCompileHeader.h"

#include "SlateTextShaper.h"

namespace DoDo {
	FSlateTextShaper::FSlateTextShaper(FFreeTypeCacheDirectory* in_ft_cache_directory, FCompositeFontCache* in_composite_font_cache, FSlateFontRenderer* in_font_renderer, FSlateFontCache* in_font_cache)
		: m_ft_cache_directory(in_ft_cache_directory)
		, m_composite_font_cache(in_composite_font_cache)
		, m_font_renderer(in_font_renderer)
		, m_font_cache(in_font_cache)
	{
	}
	FShapedGlyphSequencePtr FSlateTextShaper::shape_bidirectional_text(const DoDoUtf8String& in_text, const int32_t in_text_start, const int32_t in_text_len, const FSlateFontInfo& in_font_info, const float in_font_scale, const TextBiDi::ETextDirection in_base_direction, const ETextShapingMethod text_shaping_method) const
	{
		return FShapedGlyphSequencePtr();
	}
}