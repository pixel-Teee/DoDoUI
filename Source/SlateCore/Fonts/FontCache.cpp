#include <PreCompileHeader.h>

#include "FontCache.h"

#include "FontTypes.h"//FSlateFontKey depends on it

namespace DoDo
{
	bool FShapedGlyphEntry::has_valid_glyph() const
	{
		return true;
	}

	float FShapedGlyphEntry::get_bit_map_render_scale() const
	{
		return 1.0f;
	}

	FCharacterList::FCharacterList(const FSlateFontKey& in_font_key, FSlateFontCache& in_font_cache)
		: m_font_key(in_font_key)
		, m_font_cache(in_font_cache)
	{
	}

	FSlateFontCache::FSlateFontCache(std::shared_ptr<ISlateFontAtlasFactory> in_font_atlas_factory)
		: m_font_atlas_factory(in_font_atlas_factory)
	{
	}

	FCharacterList& FSlateFontCache::get_character_list(const FSlateFontInfo& in_font_info, float font_scale,
		const FFontOutlineSettings& in_outline_settings)
	{
		//create a key for looking up each character
		const FSlateFontKey font_key(in_font_info, in_outline_settings, font_scale);

		auto it = m_font_to_character_list_cache.find(font_key);
		
		if (it != m_font_to_character_list_cache.end())
		{
			//todo:check is stale

			return *(it->second);
		}

		//construct a FCharacterList, and instert to this map
		return *(m_font_to_character_list_cache.insert({ font_key, std::make_unique<FCharacterList>(font_key, *this) }).first->second);
	}

	int32_t FSlateFontCache::get_num_atlas_pages() const
	{
		return 0;
	}

	FSlateShaderResource* FSlateFontCache::get_atlas_page_resource(const int32_t in_index)
	{
		return nullptr;
	}

	bool FSlateFontCache::is_atlas_page_resource_alpha_only(const int32_t in_index) const
	{
		return false;
	}

	
}
