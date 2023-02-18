#pragma once

#include "VulkanInitializers.h"

#include "SlateCore/Textures/SlateShaderResource.h"

#include "SlateCore/Fonts/FontTypes.h"

namespace DoDo {
	/*
	* encapsulates a vulkan texture that can be accessed by a shader
	*/
	class FSlateVulkanTexture : public TSlateTexture<VkImageView>
	{
	public:
		FSlateVulkanTexture();

		virtual ~FSlateVulkanTexture();
		
		//todo:implement this 
		virtual uint32_t get_width() const override;

		virtual uint32_t get_height() const override;

		void set_image(const AllocatedImage& allocated_image);

		void set_shader_resource(VkImageView image_view);

		//void set_descriptor_set(VkDescriptorSet descriptor_set);
	private:
		AllocatedImage m_image;	

		//VkDescriptorSet m_descriptor_set;//every texture have one descriptor
	};

	/*
	* representation of a texture for fonts in which characters are packed tightly based on their bounding rectangle
	*/
	class FSlateFontAtlasVulkan : public FSlateFontAtlas
	{
	public:
		FSlateFontAtlasVulkan(uint32_t width, uint32_t height, const bool in_is_gray_scale);

		~FSlateFontAtlasVulkan();

	private:
		/*texture used for rendering*/
		FSlateVulkanTexture* m_font_texture;
	};
}