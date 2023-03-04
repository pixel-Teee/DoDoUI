#pragma once

#include "Core/Math/IntPoint.h"

#include "SlateFontInfo.h"//FSlateFontKey's FSlateFontInfo depends on it

#include <functional>

#include "Core/Templates/TypeHash.h"

#include "SlateCore/Textures/TextureAtlas.h"//FSlateTextureAtlas depends on it

namespace DoDo
{
	//note:font key is font info and outline settings
	struct FSlateFontKey
	{
	public:
		FSlateFontKey(const FSlateFontInfo& in_info, const FFontOutlineSettings& in_font_outline_settings, const float in_scale)
			: m_font_info(in_info)
			, m_outline_settings(in_font_outline_settings)
			, m_scale(in_scale)
			, m_key_hash(0)
		{
			//todo:calculate hash and combine hash
			m_key_hash = Hash_Combine(m_key_hash, Get_Type_Hash(m_font_info));
			m_key_hash = Hash_Combine(m_key_hash, Get_Type_Hash(m_outline_settings));
			m_key_hash = Hash_Combine(m_key_hash, Get_Type_Hash(m_scale));
		}

		bool is_identical_to_for_caching(const FSlateFontKey& other) const
		{
			return m_font_info.is_identical_to_for_caching(other.m_font_info)
				&& m_outline_settings.is_identical_to_for_caching(other.m_outline_settings)
				&& m_scale == other.m_scale;
		}

		bool operator==(const FSlateFontKey& other) const //use for collision
		{
			return is_identical_to_for_caching(other);
		}

		friend inline uint32_t Get_Type_Hash(const FSlateFontKey& key)
		{
			return key.m_key_hash;
		}

		const FSlateFontInfo& get_font_info() const
		{
			return m_font_info;
		}

		float get_scale() const
		{
			return m_scale;
		}
		
		const FFontOutlineSettings& get_font_outline_settings() const
		{
			return m_outline_settings;
		}

	private:
		FSlateFontInfo m_font_info;//font info

		FFontOutlineSettings m_outline_settings;

		float m_scale;

		uint32_t m_key_hash;
	};

	/*contains pixel data for a character rendered from freetype as well as measurement info*/
	struct FCharacterRenderData
	{
		/*raw pixels of the rendered character*/
		std::vector<uint8_t> m_raw_pixels;

		/*width of the character in pixels*/
		int16_t m_size_x = 0;

		/*height of the character in pixels*/
		int16_t m_size_y = 0;

		/*the vertical distance from the baseline to the topmost border of the glyph bitmap*/
		int16_t m_vertical_offset = 0;

		/*the horizontal distance from the origin to the leftmost border of the glyph bitmap*/
		int16_t m_horizontal_offset = 0;

		/*true if the rendered character is 8-bit grayscale, or false if it's 8-bit per-channel BGRA color*/
		bool m_b_is_gray_scale = true;

		/*true if the rendered character supports outlines, false otherwise*/
		bool m_b_supports_outline = false;
	};

	/*
	* interface to all slate font textures, both atlased and non-atlased
	*/
	class ISlateFontTexture
	{
	public:

		virtual ~ISlateFontTexture() {}

		/*
		* returns the texture resource used by slate
		*/
		virtual class FSlateShaderResource* get_slate_texture() const = 0;

		/*
		* returns whether the texture resource is 8-bit grayscale or 8-bit per-channel BGRA color
		*/
		virtual bool is_gray_scale() const = 0;
	};

	/*
	* representation of a texture for fonts in which characters are packed tightly based on their bounding rectangle
	*/
	class FSlateFontAtlas : public ISlateFontTexture, public FSlateTextureAtlas
	{
	public:

		FSlateFontAtlas(uint32_t in_width, uint32_t in_height, const bool in_is_gray_scale);

		virtual ~FSlateFontAtlas();

		//------ISlateFontTexture interface------
		virtual bool is_gray_scale() const override final;
		virtual FSlateShaderResource* get_atlas_texture() const override { return get_slate_texture(); }
		//------ISlateFontTexture interface------

		/*
		 * adds a character to the texture
		 *
		 * @param CharInfo information about the size of the character
		 */
		const struct FAtlasedTextureSlot* add_character(const FCharacterRenderData& char_info);//copy one character bitmap to runtime memory texture atlas
	};

	class ISlateFontAtlasFactory
	{
	public:
		virtual FIntPoint get_atlas_size(const bool is_in_gray_scale) const = 0;

		//virtual std::shared_ptr<FSlateFontAtlas> 
		virtual std::shared_ptr<FSlateFontAtlas> create_font_atlas(const bool in_is_gray_scale) const = 0;
	};

}

template<>
struct std::hash<DoDo::FSlateFontKey>
{
	std::size_t operator()(const DoDo::FSlateFontKey& key) const
	{
		return Get_Type_Hash(key);
	}
};
