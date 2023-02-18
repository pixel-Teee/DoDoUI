#include <PreCompileHeader.h>

#include "FontCache.h"

#include "FontTypes.h"//FSlateFontKey depends on it

namespace DoDo
{
	bool FShapedGlyphEntry::has_valid_glyph() const
	{
	}

	float FShapedGlyphEntry::get_bit_map_render_scale() const
	{
	}

	FCharacterList& FSlateFontCache::get_character_list(const FSlateFontInfo& in_font_info, float font_scale,
		const FFontOutlineSettings& in_outline_settings)
	{
		//create a key for looking up each character
		const FSlateFontKey font_key(in_font_info, in_outline_settings, font_scale);

		std::unique_ptr<FCharacterList*> cached_character_list = 
	}
}
