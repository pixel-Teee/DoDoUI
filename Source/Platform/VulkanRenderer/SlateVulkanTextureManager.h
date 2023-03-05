#pragma once

#include "SlateCore/Rendering/ShaderResourceManager.h"

#include <Vulkan/vulkan.h>

#include <functional>

#include "SlateVulkanTextures.h"//m_non_atlased_textures depends on it

#include "SlateCore/Rendering/SlateVectorGraphicsCache.h"//FSlateVectorGraphicsCache depends on it

namespace DoDo {

	class ISlateStyle;
	struct FSlateBrush;
	class FSlateShaderResourceProxy;
	class FSlateVulkanTextureAtlasFactory : public ISlateTextureAtlasFactory
	{
	public:
		virtual std::unique_ptr<FSlateTextureAtlas> create_texture_atlas(int32_t atlas_size, int32_t atlas_stride, ESlateTextureAtlasPaddingStyle padding_style, bool b_updates_after_initialization) const
		{
			return create_texture_atlas_internal(atlas_size, atlas_stride, padding_style, b_updates_after_initialization);
		}

		virtual std::unique_ptr<FSlateShaderResource> create_non_atlased_texture(const uint32_t in_width, const uint32_t in_height, const std::vector<uint8_t>& in_raw_data) const
		{
			//keep track of non-atlased textures so we can free their resources later
			std::unique_ptr<FSlateVulkanTexture> texture = std::make_unique<FSlateVulkanTexture>(in_width, in_height);

			texture->init(VK_FORMAT_R8G8B8A8_SRGB, (void*)in_raw_data.data(), true);

			return texture;
		}

		virtual void release_texture_atlases(const std::vector<std::unique_ptr<FSlateTextureAtlas>>& in_texture_atlases, const std::vector<std::unique_ptr<FSlateShaderResource>>& in_non_atlased_textures, const bool b_wait_for_release) const
		{
			//nothing to do
		}

		static std::unique_ptr<FSlateTextureAtlasVulkan> create_texture_atlas_internal(int32_t atlas_size, int32_t atlas_stride, ESlateTextureAtlasPaddingStyle padding_style, bool b_updates_after_initialization)
		{
			return std::make_unique<FSlateTextureAtlasVulkan>(atlas_size, atlas_size, atlas_stride, padding_style);
		}
	};
	/*
	* stores a mapping of texture names to their loaded vulkan resource
	* resources are loaded from disk and created on demand when needed
	*/
	class FSlateVulkanTextureManager : public FSlateShaderResourceManager
	{
	public:
		FSlateVulkanTextureManager();

		virtual ~FSlateVulkanTextureManager();

		/*
		* loads and creates rendering resources for all used textrues
		* in this implementation all textures must be known at startup time or they will not be found
		*/
		void load_used_textures();

		void load_style_resources(const ISlateStyle& style);

		/*
		* creates textures from files on disk and atlases them if possible
		* 
		* @param Resources the brush resources to load images for
		*/
		void create_textures(const std::vector<const FSlateBrush*>& resources);

		FSlateShaderResourceProxy* create_color_texture(const DoDoUtf8String texture_name, glm::vec4 in_color);

		bool load_texture(const FSlateBrush& in_brush, uint32_t& out_width, uint32_t& out_height, std::vector<uint8_t>& out_decoded_image);

		virtual FSlateShaderResourceProxy* get_shader_resource(const FSlateBrush& in_brush, glm::vec2 local_size, float draw_scale) override;

		/*
		* generates rendering resources for a texture
		* 
		* @param Info Information on how to generate the texture resource
		*/
		FSlateShaderResourceProxy* generate_texture_resource(const FNewTextureInfo& info, DoDoUtf8String texture_name);

		void update_cache();
	private:

		/*
		* gets a vector graphics resource (may generate it internally)
		* 
		* @param InBrush the brush to with texture to get
		* @param LocalSize the unscaled local size of the final image
		* @param DrawScale any scaling applied to the final image
		*/
		FSlateShaderResourceProxy* get_vector_resource(const FSlateBrush& brush, glm::vec2 local_size, float draw_scale);
		
		/*static non atlased textures*/
		std::vector<std::unique_ptr<FSlateVulkanTexture>> m_non_atlased_textures;

		/*static texture atlases*/
		std::vector<std::unique_ptr<FSlateTextureAtlasVulkan>> m_precached_texture_atlases;

		/*cache for vector graphic atlases*/
		std::unique_ptr<FSlateVectorGraphicsCache> m_vector_graphics_cache;
	};
}