#include <PreCompileHeader.h>

#include "SlateVulkanTextures.h"

namespace DoDo {
	FSlateVulkanTexture::FSlateVulkanTexture()
	{
	}
	FSlateVulkanTexture::~FSlateVulkanTexture()
	{
	}
	void FSlateVulkanTexture::set_image(const AllocatedImage& allocated_image)
	{
		m_image = allocated_image;
	}
	void FSlateVulkanTexture::set_shader_resource(VkImageView image_view)
	{
		m_shader_resource = image_view;
	}

	void FSlateVulkanTexture::update_texture(const std::vector<uint8_t>& bytes)
	{
		//todo:update texture
	}

	//void FSlateVulkanTexture::set_descriptor_set(VkDescriptorSet descriptor_set)
	//{
	//	m_descriptor_set = descriptor_set;
	//}
	uint32_t FSlateVulkanTexture::get_width() const
	{
		return 0;
	}
	uint32_t FSlateVulkanTexture::get_height() const
	{
		return 0;
	}
	FSlateFontAtlasVulkan::FSlateFontAtlasVulkan(uint32_t width, uint32_t height, const bool in_is_gray_scale)
		: FSlateFontAtlas(width, height, in_is_gray_scale)
	{
		//todo:create a font texture atlas
		
	}
	FSlateFontAtlasVulkan::~FSlateFontAtlasVulkan()
	{
	}

	void FSlateFontAtlasVulkan::conditional_update_texture()
	{
		if(m_b_needs_update)
		{
			m_font_texture->update_texture(m_atlas_data);
			m_b_needs_update = false;
		}
	}
}
