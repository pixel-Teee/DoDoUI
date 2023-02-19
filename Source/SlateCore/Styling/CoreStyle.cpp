#include <PreCompileHeader.h>

#include "CoreStyle.h"

#include "SlateCore/Fonts/LegacySlateFontCache.h"

namespace DoDo {
	std::shared_ptr<ISlateStyle> FCoreStyle::m_instance;
	bool FCoreStyle::is_star_ship_style()
	{
		return true;
	}
	std::shared_ptr<const FCompositeFont> FCoreStyle::get_default_font()
	{
		//todo:from FLegacySlateFontInfoCache
		return FLegacySlateFontInfoCache::get().get_default_font();
	}
}