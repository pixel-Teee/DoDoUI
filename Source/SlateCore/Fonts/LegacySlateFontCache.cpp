#include <PreCompileHeader.h>

#include "LegacySlateFontCache.h"

#include "Core/Misc/Paths.h"

#include "SlateCore/Fonts/CompositeFont.h"

namespace DoDo {
	std::shared_ptr<FLegacySlateFontInfoCache> FLegacySlateFontInfoCache::m_singleton;

	FLegacySlateFontInfoCache& FLegacySlateFontInfoCache::get()
	{
		if (!m_singleton)
		{
			m_singleton = std::make_shared<FLegacySlateFontInfoCache>();
		}
		return *m_singleton;
	}
	std::shared_ptr<const FCompositeFont> FLegacySlateFontInfoCache::get_default_font()
	{
		if (!m_default_font)
		{
#define APPEND_FONT(TYPEFACE, NAME, FILENAME, HINTING) \
			(TYPEFACE).append_font(NAME, FPaths::engine_content_dir() / "Slate" / "Fonts" / FILENAME, HINTING, EFontLoadingPolicy::LazyLoad)

			std::shared_ptr<FCompositeFont> mutable_default_font = std::make_shared<FStandaloneCompositeFont>();

			//default
			{
				APPEND_FONT(mutable_default_font->m_default_type_face, "Regular", "Roboto-Regular.ttf", EFontHinting::Default);
			}

#undef APPEND_FONT

			m_default_font = mutable_default_font;
		}

		return m_default_font;
	}
}