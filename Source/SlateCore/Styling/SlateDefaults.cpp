#include "PreCompileHeader.h"

#include "SlateDefaults.h"

#include "SlateCore/Brushes/SlateNoResource.h"//FSlateNoResource depends on it

#include "SlateCore/Fonts/LegacySlateFontCache.h"//FLegacySlateFontInfoCache depends on it

namespace DoDo {
	const FSlateBrush* FStyleDefaults::get_no_brush()
	{
		static const FSlateNoResource no_brush;

		return &no_brush;
	}
	const FLinearColor& FStyleDefaults::get_color()
	{
		return m_default_color;
	}

	const FSlateFontInfo FStyleDefaults::get_font_info(uint16_t size)
	{
		return FSlateFontInfo(FLegacySlateFontInfoCache::get().get_default_font(), size, "Regular");
	}

	FLinearColor FStyleDefaults::m_default_color = FLinearColor(1, 1, 1);
}