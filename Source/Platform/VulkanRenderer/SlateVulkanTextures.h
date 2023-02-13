#pragma once

#include "VulkanInitializers.h"

#include "SlateCore/Textures/SlateShaderResource.h"

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
}