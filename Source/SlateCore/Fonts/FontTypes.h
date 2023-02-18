#pragma once

#include "Core/Math/IntPoint.h"

#include "SlateFontInfo.h"//FSlateFontKey's FSlateFontInfo depends on it

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
		}

	private:
		FSlateFontInfo m_font_info;//font info

		FFontOutlineSettings m_outline_settings;

		float m_scale;

		uint32_t m_key_hash;
	};

	class ISlateFontAtlasFactory
	{
	public:
		virtual FIntPoint get_atlas_size(const bool is_in_gray_scale) const = 0;

		//virtual std::shared_ptr<FSlateFontAtlas> 
	};
}
