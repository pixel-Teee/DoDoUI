#pragma once

#include "SlateCore/Textures/SlateShaderResource.h"

#include "vulkan/vulkan.hpp"//todo:remove this

namespace DoDo {
	/*
	* encapsulates a render target for use by a slate rendering implementation
	*/
	class FSlateRenderTargetRHI : public TSlateTexture<VkImageView> //todo:fix this
	{
	public:
		FSlateRenderTargetRHI(VkImageView in_render_target_texture, uint32_t in_width, uint32_t in_height)
			: TSlateTexture(in_render_target_texture)
			, m_width(in_width)
			, m_height(in_height)
		{}

		//todo:add more functions

		virtual uint32_t get_width() const override { return m_width; }
		virtual uint32_t get_height() const override { return m_height; }

	private:

		/*width of this texture*/
		uint32_t m_width;

		/*height of this texture*/
		uint32_t m_height;
	};
}