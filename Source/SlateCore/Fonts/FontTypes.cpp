#include <PreCompileHeader.h>

#include "FontTypes.h"

namespace DoDo {
	FSlateFontAtlas::FSlateFontAtlas(uint32_t in_width, uint32_t in_height, const bool in_is_gray_scale)
		: FSlateTextureAtlas(in_width, in_height, in_is_gray_scale ? 1 : 4)
	{
	}
	FSlateFontAtlas::~FSlateFontAtlas()
	{
	}

	const FAtlasedTextureSlot* FSlateFontAtlas::add_character(const FCharacterRenderData& render_data)
	{
		return add_texture(render_data.m_size_x, render_data.m_size_y, render_data.m_raw_pixels);//note:add to runtime memory
	}
}

