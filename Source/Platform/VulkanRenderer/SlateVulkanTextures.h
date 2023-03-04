#pragma once

#include "VulkanInitializers.h"

#include "SlateCore/Textures/SlateShaderResource.h"

#include "SlateCore/Fonts/FontTypes.h"

#include "SlateCore/Textures/SlateUpdatableTexture.h"//FSlateUpdatableTexture depends on it

#include "Utils.h"//AllocatedBuffer depends on it

namespace DoDo {
	struct FIntRect;
	/*
	* encapsulates a vulkan texture that can be accessed by a shader
	*/
	class FSlateVulkanTexture : public TSlateTexture<VkImageView>, public FSlateUpdatableTexture
	{
	public:
		FSlateVulkanTexture();

		FSlateVulkanTexture(uint32_t in_size_x, uint32_t in_size_y)
			: TSlateTexture()
			, m_size_x(in_size_x)
			, m_size_y(in_size_y)
		{}

		virtual ~FSlateVulkanTexture();
		
		//todo:implement this 
		virtual uint32_t get_width() const override;

		virtual uint32_t get_height() const override;

		void set_image(const AllocatedImage& allocated_image);

		void set_shader_resource(VkImageView image_view);

		virtual void update_texture(const std::vector<uint8_t>& bytes) override;

		void init(VkFormat in_format, void* initial_data, bool b_use_staging_texture);

		//void set_descriptor_set(VkDescriptorSet descriptor_set);
	private:
		/*
		* helper method used by the different update texture* methods
		*/
		void update_texture_raw(const void* buffer, const FIntRect& dirty);

		AllocatedBuffer m_staging_buffer;

		AllocatedImage m_image;	

		uint32_t m_size_x;

		uint32_t m_size_y;

		uint32_t m_bytes_per_pixel;

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

		virtual class FSlateShaderResource* get_slate_texture() const override { return m_font_texture; }

		void conditional_update_texture() override;
	private:
		/*texture used for rendering*/
		FSlateVulkanTexture* m_font_texture;
	};

	class FSlateTextureAtlasVulkan : public FSlateTextureAtlas
	{
	public:
		FSlateTextureAtlasVulkan(uint32_t width, uint32_t height, uint32_t stride_bytes, ESlateTextureAtlasPaddingStyle padding_style);
		~FSlateTextureAtlasVulkan();

		virtual void conditional_update_texture() override;
		
		FSlateVulkanTexture* get_atlas_texture() const { return m_atlas_texture; }
	private:
		void init_atlas_texture();
	private:
		FSlateVulkanTexture* m_atlas_texture;
	};
}
