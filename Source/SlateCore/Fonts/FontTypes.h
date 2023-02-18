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

	private:
		FSlateFontInfo m_font_info;//font info

		FFontOutlineSettings m_outline_settings;

		float m_scale;

		uint32_t m_key_hash;
	};

	/*
	* interface to all slate font textures, both atlased and non-atlased
	*/
	class ISlateFontTexture
	{
	public:
		virtual ~ISlateFontTexture() {}
	};

	/*
	* representation of a texture for fonts in which characters are packed tightly based on their bounding rectangle
	*/
	class FSlateFontAtlas : public ISlateFontTexture, public FSlateTextureAtlas
	{
	public:
		FSlateFontAtlas(uint32_t in_width, uint32_t in_height, const bool in_is_gray_scale);

		virtual ~FSlateFontAtlas();
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
