#include <PreCompileHeader.h>

#include "FontCache.h"

#include "FontTypes.h"//FSlateFontKey depends on it

#include "SlateFontRenderer.h"//get_max_character_height depends on it

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
		, m_max_direct_indexed_entries(256)
	{
	}

	FCharacterEntry FCharacterList::get_character(char character, const EFontFallback max_font_fall_back)
	{
		const FCharacterListEntry* internal_entry = nullptr;
		const bool b_direct_index_char = character < m_max_direct_indexed_entries;

		//first get a reference to the character, if it is already mapped (mapped does not mean cached though)
		if(b_direct_index_char)
		{
			if(character >= 0 && character < m_direct_index_entries.size())
			{
				internal_entry = &m_direct_index_entries[character];
			}
		}
		else
		{
			auto it = m_mapped_entries.find(character);
			if(it != m_mapped_entries.end())
			{
				internal_entry = &(it->second);
			}
		}

		//note:cache character function will call add new entry
		bool b_need_caching = false;

		if (internal_entry)
		{
			b_need_caching = !internal_entry->m_valid;//has already initialized?

			//if the character needs caching, but can't be cached, reject the character
			if (b_need_caching && !can_cache_character(character, max_font_fall_back))
			{
				b_need_caching = false;
				internal_entry = nullptr;
			}
		}
		//only map the character if it can be cached
		else if (can_cache_character(character, max_font_fall_back))
		{
			b_need_caching = true;

			if (b_direct_index_char)
			{
				//insert n elements
				m_direct_index_entries.insert(m_direct_index_entries.end(), (character - m_direct_index_entries.size()) + 1, {});
				internal_entry = &m_direct_index_entries[character];
			}
			else
			{
				auto it = m_mapped_entries.insert({ character, FCharacterListEntry() });
				internal_entry = &(it.first->second);
			}
		}

		if (internal_entry)
		{
			if (b_need_caching)
			{
				//internal_entry = cache_character(character);
				//todo:implement this function
			}
		}

		if (internal_entry)
		{
			return make_character_entry(character, *internal_entry);
		}

		return FCharacterEntry{};
	}

	uint16_t FCharacterList::get_max_height() const
	{
		if(m_max_height == 0)
		{
			m_max_height = m_font_cache.get_max_character_height(m_font_key.get_font_info(), m_font_key.get_scale());//note:via the slate font cache pass to font renderer
		}

		return m_max_height;
	}

	bool FCharacterList::can_cache_character(char character, const EFontFallback max_font_fall_back) const
	{
		bool b_retuan_val = false;

		//todo:check invalid

		//todo:implement get font data for code point

		//todo:implement can load code point

		return true;
	}

	FCharacterEntry FCharacterList::make_character_entry(char character, const FCharacterListEntry& internal_entry) const
	{
		FCharacterEntry char_entry;

		char_entry.m_valid = internal_entry.m_valid;

		if (char_entry.m_valid)
		{
			if (char_entry.m_valid)
			{
				char_entry.character = character;

				char_entry.m_glyph_index = internal_entry.m_shaped_glyph_entry.m_graph_index;

				//todo:implement font data

				//todo:implement kerning cache

				//char_entry.m_font_scale = internal_entry.m_shaped_glyph_entry.m_font_face_data->

				//char_entry.m_start_u =

				char_entry.m_x_advance = internal_entry.m_shaped_glyph_entry.m_x_advance;

				char_entry.m_has_kerning = internal_entry.m_has_kerning;
			}
		}

		return char_entry;
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

		//construct a FCharacterList, and insert to this map
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

	void FSlateFontCache::update_cache()
	{
		auto update_font_atlas_textures = [this](const std::vector<uint8_t>& font_atlas_indices)
		{
			for(const uint8_t font_atlas_index : font_atlas_indices)
			{
				FSlateFontAtlas& font_atlas = static_cast<FSlateFontAtlas&>(*m_all_font_textures[font_atlas_index]);
				font_atlas.conditional_update_texture();//note:update runtime shader texture
			}
		};

		update_font_atlas_textures(m_gray_scale_font_atlas_indices);

		update_font_atlas_textures(m_color_font_atlas_indices);

		//todo:implement composite font cache
	}

	uint16_t FSlateFontCache::get_max_character_height(const FSlateFontInfo& in_font_info, float font_scale) const//note:called by the FCharacterList
	{
		return m_font_renderer->get_max_height(in_font_info, font_scale);
	}
}
