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
		, m_max_height(0)
		, m_max_direct_indexed_entries(256)
	{
	}

	FCharacterEntry FCharacterList::get_character(DoDoUtf8String character, const EFontFallback max_font_fall_back)
	{
		const FCharacterListEntry* internal_entry = nullptr;
		const bool b_direct_index_char = character.to_utf16_code_point(0) < m_max_direct_indexed_entries;

		//first get a reference to the character, if it is already mapped (mapped does not mean cached though)
		if(b_direct_index_char)
		{
			if(character.to_utf16_code_point(0) >= 0 && character.to_utf16_code_point(0) < m_direct_index_entries.size())
			{
				internal_entry = &m_direct_index_entries[character.to_utf16_code_point(0)];
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
				m_direct_index_entries.insert(m_direct_index_entries.end(), (character.to_utf16_code_point(0) - m_direct_index_entries.size()) + 1, {});
				internal_entry = &m_direct_index_entries[character.to_utf16_code_point(0)];
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
				internal_entry = cache_character(character);//construct a FCharacterListEntry
				//todo:implement this function
			}
		}

		if (internal_entry)
		{
			return make_character_entry(character, *internal_entry);//use FCharacterListEntry to construct a FCharacterEntry
		}

		return FCharacterEntry{};
	}

	uint16_t FCharacterList::get_max_height() const
	{
		//when constructor a FCharacterList, FCharacterList will record FSlateFontCache reference
		if(m_max_height == 0)
		{
			m_max_height = m_font_cache.get_max_character_height(m_font_key.get_font_info(), m_font_key.get_scale());//note:via the slate font cache pass to font renderer
		}

		return m_max_height;
	}

	bool FCharacterList::can_cache_character(DoDoUtf8String character, const EFontFallback max_font_fall_back) const
	{
		bool b_retuan_val = false;

		//todo:check invalid

		//todo:implement get font data for code point

		//todo:implement can load code point

		return true;
	}

	FCharacterList::FCharacterListEntry* FCharacterList::cache_character(DoDoUtf8String character)
	{
		const FSlateFontInfo& font_info = m_font_key.get_font_info();

		//get the data needed to render this character
		float sub_font_scaling_factor = 1.0f;
		const FFontData* font_data_ptr = &m_font_cache.m_composite_font_cache->get_font_data_for_code_point(font_info, character.to_utf16_code_point(0), sub_font_scaling_factor);
		FFreeTypeFaceGlyphData face_glyph_data = m_font_cache.m_font_renderer->get_font_face_for_code_point(*font_data_ptr, character.to_utf16_code_point(0), font_info.m_font_fallback);
		//note:this is internal struct to use for load the free type
		if(face_glyph_data.m_face_and_memory->is_face_valid())
		{
			const float final_font_scale = m_font_key.get_scale() * sub_font_scaling_factor;

			uint32_t glyph_flags = 0;
			//todo:append glyph flags
			//note:add additional flags
			SlateFontRendererUtils::append_glyph_flags(*face_glyph_data.m_face_and_memory, *font_data_ptr, glyph_flags);

			const bool b_has_kerning = FT_HAS_KERNING(face_glyph_data.m_face_and_memory->get_face()) != 0;

			const bool b_is_white_space = false;//todo:check is white space

			const uint32_t glyph_index = FT_Get_Char_Index(face_glyph_data.m_face_and_memory->get_face(), character.to_utf16_code_point(0));//to get the glyph index (glyph index can access character texture)

			int16_t x_advance = 0;
			{
				FT_Fixed cached_advance_data = 0;
				std::shared_ptr<FFreeTypeAdvanceCache> advance_cache = m_font_cache.m_ft_cache_directory->get_advance_cache(face_glyph_data.m_face_and_memory->get_face(), glyph_flags,
					font_info.m_size, final_font_scale);

				if (advance_cache->find_or_cache(glyph_index, cached_advance_data))
				{
					x_advance = FreeTypeUtils::Convert26Dot6ToRoundedPixel<int16_t>((cached_advance_data + (1 << 9)) >> 10);
				}
			}

			//todo:fix me, use font_info.m_size
			FCharacterListEntry new_internal_entry;
			new_internal_entry.m_shaped_glyph_entry.m_font_face_data = std::make_shared<FShapedGlyphFaceData>(face_glyph_data.m_face_and_memory, glyph_flags, font_info.m_size, final_font_scale);
			new_internal_entry.m_shaped_glyph_entry.m_graph_index = glyph_index;
			new_internal_entry.m_shaped_glyph_entry.m_x_advance = x_advance;
			new_internal_entry.m_shaped_glyph_entry.m_b_is_visible = !b_is_white_space;

			new_internal_entry.m_font_data = font_data_ptr;
			new_internal_entry.m_fall_back_level = face_glyph_data.m_char_fall_back_level;
			new_internal_entry.m_has_kerning = b_has_kerning;
			new_internal_entry.m_valid = character == 0 || glyph_index != 0;

			//cache the shaped entry in the font cache
			if(new_internal_entry.m_valid)
			{
				m_font_cache.get_shaped_glyph_font_atlas_data(new_internal_entry.m_shaped_glyph_entry, m_font_key.get_font_outline_settings());

				if(character.to_utf16_code_point(0) < m_max_direct_indexed_entries)
				{
					m_direct_index_entries[character.to_utf16_code_point(0)] = std::move(new_internal_entry);
					return &m_direct_index_entries[character.to_utf16_code_point(0)];
				}
				else
				{
					//auto it = m_mapped_entries.insert({ character, std::move(new_internal_entry) });
					//it.first->second = new_internal_entry;
					//return &(it.first->second);
					auto it = m_mapped_entries.find(character);
					if (it != m_mapped_entries.end())
					{
						it->second = new_internal_entry;
					}

					return  &(it->second);
				}
			}
		}

		return nullptr;
	}

	FCharacterEntry FCharacterList::make_character_entry(DoDoUtf8String character, const FCharacterListEntry& internal_entry) const
	{
		FCharacterEntry char_entry;//interms of FCharacterListEntry to construct a FCharacterEntry

		char_entry.m_valid = internal_entry.m_valid;

		if (char_entry.m_valid)
		{
			//use FShapedGlyphFontAtlasData and FCharacterListEntry to construct a FCharacterEntry
			FShapedGlyphFontAtlasData shaped_glyph_font_atlas_data = m_font_cache.get_shaped_glyph_font_atlas_data(internal_entry.m_shaped_glyph_entry, 
			m_font_key.get_font_outline_settings());

			if (char_entry.m_valid)
			{
				char_entry.character = character;

				char_entry.m_glyph_index = internal_entry.m_shaped_glyph_entry.m_graph_index;//access the character at the freetype texture

				//todo:implement font data
				char_entry.m_font_data = internal_entry.m_font_data;

				//todo:implement kerning cache
				

				char_entry.m_font_scale = internal_entry.m_shaped_glyph_entry.m_font_face_data->m_font_scale;

				char_entry.m_bitmap_render_scale = internal_entry.m_shaped_glyph_entry.m_font_face_data->m_bit_map_render_scale;

				char_entry.m_start_u = shaped_glyph_font_atlas_data.m_start_u;

				char_entry.m_start_v = shaped_glyph_font_atlas_data.m_start_v;

				char_entry.m_u_size = shaped_glyph_font_atlas_data.m_u_size;

				char_entry.m_v_size = shaped_glyph_font_atlas_data.m_v_size;

				char_entry.m_vertical_offset = shaped_glyph_font_atlas_data.m_vertical_offset;

				char_entry.m_horizontal_offset = shaped_glyph_font_atlas_data.m_horizontal_offset;

				char_entry.m_global_descendar = 0.0f;//todo:implement get base line

				char_entry.m_x_advance = internal_entry.m_shaped_glyph_entry.m_x_advance;

				char_entry.m_texture_index = shaped_glyph_font_atlas_data.m_texture_index;

				char_entry.m_has_kerning = internal_entry.m_has_kerning;

				char_entry.m_supports_outline = shaped_glyph_font_atlas_data.m_supports_outline;

				char_entry.m_fall_back_level = internal_entry.m_fall_back_level;
			}
		}

		return char_entry;
	}

	FSlateFontCache::FSlateFontCache(std::shared_ptr<ISlateFontAtlasFactory> in_font_atlas_factory)
		: m_ft_library(new FFreeTypeLibrary())
		, m_ft_cache_directory(new FFreeTypeCacheDirectory())
		, m_composite_font_cache(new FCompositeFontCache(m_ft_library.get()))
		, m_font_atlas_factory(in_font_atlas_factory)
		, m_font_renderer(new FSlateFontRenderer(m_ft_library.get(), m_composite_font_cache.get()))
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

	FShapedGlyphFontAtlasData FSlateFontCache::get_shaped_glyph_font_atlas_data(const FShapedGlyphEntry& in_shaped_glyph, const FFontOutlineSettings& in_outline_settings)
	{
		uint8_t cached_type_index = (uint8_t)(in_outline_settings.m_outline_size <= 0 ? EFontCacheAtlasDataType::Regular : EFontCacheAtlasDataType::Outline);

		const int32_t cached_atlas_data_thread_index = static_cast<int32_t>(0);//

		//has the atlas data already been cached on the glyph?
		{
			std::shared_ptr<FShapedGlyphFontAtlasData> cached_atlas_data_pin = in_shaped_glyph.m_cached_atlas_data[cached_type_index][cached_atlas_data_thread_index].lock();

			if (cached_atlas_data_pin)
			{
				return *cached_atlas_data_pin;
			}
		}

		//not cached on the glyph, so create a key for to look up this glyph, as it may
		//have already been cached by another shaped text sequence
		const FShapedGlyphEntryKey glyph_key(*in_shaped_glyph.m_font_face_data, in_shaped_glyph.m_graph_index, in_outline_settings);
		{
			//not cached at all... create a new entry
			std::shared_ptr<FShapedGlyphFontAtlasData> new_atlas_data = std::make_shared<FShapedGlyphFontAtlasData>();
			add_new_entry(in_shaped_glyph, in_outline_settings, *new_atlas_data);//note:this function is important, will use freetype to render a character bitmap

			if (new_atlas_data->m_valid)//already initialized?
			{
				in_shaped_glyph.m_cached_atlas_data[cached_type_index][cached_atlas_data_thread_index] = new_atlas_data;
				m_shaped_glyph_to_atlas_data.insert({ glyph_key, new_atlas_data });//interms FShapedGlyphEntry to construct FShapedGlyphEntryKey, map FShapedGlyphFontAtlasData
			}

			return *new_atlas_data;
		}

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

	bool FSlateFontCache::add_new_entry(const FShapedGlyphEntry& in_shaped_glyph, const FFontOutlineSettings& in_outline_settings, FShapedGlyphFontAtlasData& out_atlas_data)
	{
		//render the glyph
		FCharacterRenderData render_data;

		const bool b_did_render = m_font_renderer->get_render_data(in_shaped_glyph, in_outline_settings, render_data);

		//note:use add new entry to populate the information to the FShapedGlyphFontAtlasData
		//note:this function will add the character to the texture atlas (runtime video texture), add get the position and width/height to populate the output parameter
		out_atlas_data.m_valid = b_did_render && add_new_entry(render_data, out_atlas_data.m_texture_index, out_atlas_data.m_start_u, out_atlas_data.m_start_v,
			out_atlas_data.m_u_size, out_atlas_data.m_v_size);

		if(out_atlas_data.m_valid)
		{
			out_atlas_data.m_vertical_offset = render_data.m_vertical_offset;
			out_atlas_data.m_horizontal_offset = render_data.m_horizontal_offset;
			out_atlas_data.m_supports_outline = render_data.m_b_supports_outline;
		}

		return out_atlas_data.m_valid;//initialized
	}

	bool FSlateFontCache::add_new_entry(const FCharacterRenderData in_render_data, uint8_t& out_texture_index,
		uint16_t& out_glyph_x, uint16_t& out_glyph_y, uint16_t& out_glyph_width, uint16_t& out_glyph_height)
	{
		//will this entry fit within any atlas texture
		const FIntPoint font_atlas_size = m_font_atlas_factory->get_atlas_size(in_render_data.m_b_is_gray_scale);//1024 * 1024

		if(in_render_data.m_size_x > font_atlas_size.x || in_render_data.m_size_y > font_atlas_size.y)//note:too big
		{
			return false;
		}

		//reference capture
		auto fill_output_params_from_atlased_texture_slot = [&](const FAtlasedTextureSlot& atlased_texture_slot)
		{
			int32_t glyph_x = atlased_texture_slot.m_x + (int32_t)atlased_texture_slot.m_padding;//note:position and width/height
			int32_t glyph_y = atlased_texture_slot.m_y + (int32_t)atlased_texture_slot.m_padding;
			int32_t glyph_width = atlased_texture_slot.m_width - (int32_t)(2.0f * atlased_texture_slot.m_padding);
			int32_t glyph_height = atlased_texture_slot.m_height - (int32_t)(2.0f * atlased_texture_slot.m_padding);

			out_glyph_x = (uint16_t)glyph_x;
			out_glyph_y = (uint16_t)glyph_y;
			out_glyph_width = (uint16_t)glyph_width;
			out_glyph_height = (uint16_t)glyph_height;
		};

		std::vector<uint8_t>& font_atlas_indices = in_render_data.m_b_is_gray_scale ? m_gray_scale_font_atlas_indices : m_color_font_atlas_indices;

		for(const uint8_t font_atlas_index : font_atlas_indices)
		{
			FSlateFontAtlas& font_atlas = static_cast<FSlateFontAtlas&>(*m_all_font_textures[font_atlas_index]);

			//add the character to the texture
			const FAtlasedTextureSlot* new_slot = font_atlas.add_character(in_render_data);//note:this function is important

			if(new_slot)
			{
				out_texture_index = font_atlas_index;
				fill_output_params_from_atlased_texture_slot(*new_slot);//populate the output parameter
				return true;
			}
		}

		//------create font atlas------
		std::shared_ptr<FSlateFontAtlas> font_atlas = m_font_atlas_factory->create_font_atlas(in_render_data.m_b_is_gray_scale);
		m_all_font_textures.push_back(font_atlas);
		out_texture_index = m_all_font_textures.size() - 1;

		font_atlas_indices.push_back(out_texture_index);//note:this is important, will be used to update runtime texture

		//add the character to the texture
		const FAtlasedTextureSlot* new_slot = font_atlas->add_character(in_render_data);
		if(new_slot)
		{
			fill_output_params_from_atlased_texture_slot(*new_slot);
		}

		return new_slot != nullptr;
		//------create font atlas------
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
		//font renderer is bridging point between FreeType and the Slate font system
		return m_font_renderer->get_max_height(in_font_info, font_scale);
	}

	bool FSlateFontCache::flush_cache()
	{
		m_gray_scale_font_atlas_indices.clear();
		m_color_font_atlas_indices.clear();
		return true;
	}

	FShapedGlyphEntryKey::FShapedGlyphEntryKey(const FShapedGlyphFaceData& in_font_face_data, uint32_t in_glyph_index, const FFontOutlineSettings& in_outline_settings)
		: m_font_face(in_font_face_data.m_font_face)
		, m_font_size(in_font_face_data.m_font_size)
		, m_outline_size(in_outline_settings.m_outline_size)
		, m_font_scale(in_font_face_data.m_font_scale)
		, m_glyph_index(in_glyph_index)
		, m_key_hash(0)
	{
		//todo:calculate key hash
		//m_key_hash = Hash_Combine(m_key_hash, Get_Type_Hash(m_font_face));
		m_key_hash = Hash_Combine(m_key_hash, Get_Type_Hash(m_font_size));
		m_key_hash = Hash_Combine(m_key_hash, Get_Type_Hash(m_outline_size));
		m_key_hash = Hash_Combine(m_key_hash, Get_Type_Hash(m_font_scale));
		//m_key_hash = Hash_Combine(m_key_hash, Get_Type_Hash(m_outline_separate_fill_alpha));
		m_key_hash = Hash_Combine(m_key_hash, Get_Type_Hash(m_glyph_index));
	}
	FShapedGlyphSequence::FShapedGlyphSequence(std::vector<FShapedGlyphEntry> in_glyphs_to_render, const int16_t in_text_base_line, const uint16_t in_max_text_height, const FFontOutlineSettings& in_outline_settings, const FSourceTextRange& in_source_text_range)
		: m_glyphs_to_render(std::move(in_glyphs_to_render))
		, m_text_base_line(in_text_base_line)
		, m_max_text_height(in_max_text_height)
		, m_outline_settings(in_outline_settings)
		, m_sequence_width(0)
		, m_source_indices_to_glyph_data(in_source_text_range)
	{
		const int32_t num_glyphs_to_render = m_glyphs_to_render.size();

		for (int32_t current_glyph_index = 0; current_glyph_index < num_glyphs_to_render; ++current_glyph_index)
		{
			const FShapedGlyphEntry& current_glyph = m_glyphs_to_render[current_glyph_index];

			//track unique font faces
			if (current_glyph.m_font_face_data->m_font_face.lock())
			{
				//note:add unique
				for (size_t j = 0; j < m_glyph_font_faces.size(); ++j)
				{
					if (m_glyph_font_faces[j].lock() == current_glyph.m_font_face_data->m_font_face.lock())
					{
						break;
					}
				}

				//note:add unique
				m_glyph_font_faces.push_back(current_glyph.m_font_face_data->m_font_face);
			}

			//update the measured width
			m_sequence_width += current_glyph.m_x_advance;

			//note:m_source_index is this glyph in original text position
			FSourceIndexToGlyphData* source_index_to_glyph_data = m_source_indices_to_glyph_data.get_glyph_data(current_glyph.m_source_index);

			//skip if index is invalid or hidden
			if (!source_index_to_glyph_data)
			{
				//todo:add more logic
			}

			if (source_index_to_glyph_data->is_valid())
			{
				//if this data already exists then it means a single character produced multiple glyphs and we need to track it as an additional glyph(these are always within the same cluster block)
				source_index_to_glyph_data->m_additional_glyph_indices.push_back(current_glyph_index);
			}
			else
			{
				*source_index_to_glyph_data = FSourceIndexToGlyphData(current_glyph_index);
			}
		}
	}
	FShapedGlyphSequence::~FShapedGlyphSequence()
	{
	}
	int32_t FShapedGlyphSequence::get_measured_width() const
	{
		return m_sequence_width;
	}
	std::optional<int32_t> FShapedGlyphSequence::get_measured_width(const int32_t in_start_index, const int32_t in_end_index, const bool in_include_kerning_with_preceding_glyph) const
	{
		int32_t measured_width = 0;

		if (in_include_kerning_with_preceding_glyph && in_start_index > 0)
		{
			const std::optional<int8_t> kerning = get_kerning(in_start_index - 1);

			measured_width += kerning.value_or(0);
		}

		auto glyph_call_back = [&](const FShapedGlyphEntry& current_glyph, int32_t current_glyph_index)->bool
		{
			measured_width += current_glyph.m_x_advance;
			return true;
		};


		//todo:add enumerate logical glyphs in source range
		return measured_width;
		
		return std::optional<int32_t>();
	}
	std::optional<int8_t> FShapedGlyphSequence::get_kerning(const int32_t in_index) const
	{
		const FSourceIndexToGlyphData* source_index_to_glyph_data = m_source_indices_to_glyph_data.get_glyph_data(in_index);
		if (source_index_to_glyph_data && source_index_to_glyph_data->is_valid())
		{
			const FShapedGlyphEntry& current_glyph = m_glyphs_to_render[source_index_to_glyph_data->m_glyph_index];
			return current_glyph.m_kerning;
		}

		//if we got herte it means we couldn't find the glyph
		return std::optional<int8_t>();
	}
}
