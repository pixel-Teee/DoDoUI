#pragma once

#include "SlateCore/Rendering/ShaderResourceManager.h"

#include <Vulkan/vulkan.h>

#include <functional>

#include "SlateVulkanTextures.h"//m_non_atlased_textures depends on it

namespace DoDo {

	class ISlateStyle;
	struct FSlateBrush;
	class FSlateShaderResourceProxy;
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
	private:
		
		/*static non atlased textures*/
		std::vector<std::unique_ptr<FSlateVulkanTexture>> m_non_atlased_textures;
	};
}