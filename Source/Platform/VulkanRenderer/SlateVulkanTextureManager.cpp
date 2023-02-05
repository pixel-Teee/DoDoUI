#include <PreCompileHeader.h>

#include "SlateVulkanTextureManager.h"

#include "SlateCore/Styling/SlateBrush.h"

#include "SlateCore/Styling/SlateStyle.h"

#include "SlateCore/Textures/SlateShaderResource.h"

#include "stb_image.h"

#include "Application/Application.h"

#include "SlateVulkanRenderer.h"

#include "Utils.h"

#include "VulkanInitializers.h"

#include "Renderer/Device.h"

#include "SlateCore/Styling/SlateStyleRegistry.h"

namespace DoDo {

	FSlateVulkanTextureManager::FSlateVulkanTextureManager()
	{
		//std::vector<const FSlateBrush*> resources;

	
		//todo:create 
	}
	FSlateVulkanTextureManager::~FSlateVulkanTextureManager()
	{
	}
	void FSlateVulkanTextureManager::load_used_textures()
	{
		std::vector<const FSlateBrush*> resources;

		FSlateStyleRegistry::get_all_resources(resources);

		create_textures(resources);
	}
	void FSlateVulkanTextureManager::load_style_resources(const ISlateStyle& style)
	{
		std::vector<const FSlateBrush*> resources;

		style.get_resources(resources);//from FlateStyleSet to collect all slate brush

		//to create real texture
		create_textures(resources);
	}
	void FSlateVulkanTextureManager::create_textures(const std::vector<const FSlateBrush*>& resources)
	{
		std::map<DoDoUtf8String, FNewTextureInfo> texture_info_map;

		for (int32_t resource_index = 0; resource_index < resources.size(); ++resource_index)//create real gpu buffer
		{
			const FSlateBrush& brush = *resources[resource_index];//todo:get the brush

			const DoDoUtf8String texture_name = brush.get_resource_name();

			if (brush.get_image_type() != ESlateBrushImageType::Vector && texture_name != "")//todo:implement resource map
			{
				//find the texture or add it if it doesn't exist(only load the texture once)
				auto it = texture_info_map.find(texture_name);
				if (it == texture_info_map.end())
				{
					//create a new texture info
					std::pair<std::map<DoDoUtf8String, FNewTextureInfo>::iterator, bool> ret_result = texture_info_map.insert({ texture_name, FNewTextureInfo() });

					it = ret_result.first;//get the iterator

					FNewTextureInfo& info = (it->second);

					info.m_b_srgb = (brush.m_image_type != ESlateBrushImageType::Linear);

					//only atlas the texture if none of the brushes that use it tile it
					info.m_b_should_atlas &= (brush.m_tiling == ESlateBrushTileType::NoTile && info.m_b_srgb);//todo:how to understand it?

					if (info.m_texture_data == nullptr)
					{
						//create memory
						uint32_t width = 0;
						uint32_t height = 0;
						std::vector<uint8_t> raw_data;
						bool b_succeeded = load_texture(brush, width, height, raw_data);//main memory

						const uint32_t stride = 4;//rgba

						info.m_texture_data = std::make_shared<FSlateTextureData>(width, height, stride, raw_data);

						const bool b_too_large_for_atlas = (width >= 256 || height >= 256);

						info.m_b_should_atlas &= !b_too_large_for_atlas;

						//success check
					}
				}
			}
		}

		//value sort?

		for(std::map<DoDoUtf8String, FNewTextureInfo>::const_iterator it = texture_info_map.begin(); it != texture_info_map.end(); ++it)
		{
			const FNewTextureInfo& info = it->second;
			
			DoDoUtf8String texture_name = it->first;

			//todo:implement generate texture resource(this is real place to generate gpu buffer)

			FSlateShaderResourceProxy* new_texture = generate_texture_resource(info, texture_name);

			m_resource_map.insert({ texture_name, new_texture });
		}
	}
	bool FSlateVulkanTextureManager::load_texture(const FSlateBrush& in_brush, uint32_t& out_width, uint32_t& out_height, std::vector<uint8_t>& out_decoded_image)
	{
		DoDoUtf8String resource_path = get_resource_path(in_brush);//assume the brush contains the whole name

		uint32_t bytes_per_pixel = 4;

		bool b_succeeded = false;
		//std::vector<uint8_t> raw_file_data;

		int32_t x = 0, y = 0, n = 0;
		unsigned char* data = stbi_load(resource_path.c_str(), &x, &y, &n, 4);

		if (data != nullptr)
		{
			//load success
			out_width = x;
			out_height = y;

			//todo:check 4 component

			out_decoded_image.resize(x * y * 4);//todo:need png

			std::memcpy(out_decoded_image.data(), data, x * y * 4);//todo:fix me

			//todo:only png support in slate
			stbi_image_free(data);
			return true;
		}

		return false;
	}
	FSlateShaderResourceProxy* FSlateVulkanTextureManager::get_shader_resource(const FSlateBrush& in_brush, glm::vec2 local_size, float draw_scale)
	{
		FSlateShaderResourceProxy* texture = nullptr;

		if (in_brush.get_image_type() == ESlateBrushImageType::Vector)
		{
			//todo:
		}
		else//todo:implement dynamic resource
		{
			auto it = m_resource_map.find(in_brush.get_resource_name());
			texture = it->second;
		}

		return texture;
	}
	FSlateShaderResourceProxy* FSlateVulkanTextureManager::generate_texture_resource(const FNewTextureInfo& info, DoDoUtf8String texture_name)
	{
		FSlateShaderResourceProxy* new_proxy = nullptr;

		const uint32_t width = info.m_texture_data->get_width();
		const uint32_t height = info.m_texture_data->get_height();

		if (info.m_b_should_atlas)
		{
			const uint32_t atlas_size = 1024;

			//4 bytes per pixel
			const uint32_t atlas_stride = 4;

			//
		}
		else
		{
			//the texture is not atlased create a new texture proxy and just point it to the actual texture
			new_proxy = new FSlateShaderResourceProxy;

			//keep track of non-atlased textures so we can free their resource later
			//todo:implement vulkan texture
			std::unique_ptr<FSlateVulkanTexture> texture = std::make_unique<FSlateVulkanTexture>();

			new_proxy->m_resource = texture.get();
			new_proxy->m_start_uv = glm::vec2(0.0f, 0.0f);
			new_proxy->m_size_uv = glm::vec2(1.0f, 1.0f);
			new_proxy->m_actual_size = FIntPoint(width, height);

			//------------------init texture------------------

			//get renderer
			Renderer* renderer = Application::get().get_renderer();
			FSlateVulkanRenderer* vulkan_renderer = (FSlateVulkanRenderer*)(renderer);

			//the format R8G8B8A8 matches exactly with the pixels loaded from stb_image lib
			VkFormat image_format = VK_FORMAT_R8G8B8A8_SRGB;

			//allocate temporary buffer for holding texture data to upload
			AllocatedBuffer staging_buffer = vulkan_renderer->create_buffer(width * height * 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

			//copy data to buffer
			void* data;
			vmaMapMemory(vulkan_renderer->m_allocator, staging_buffer.m_allocation, &data);

			memcpy(data, info.m_texture_data->get_raw_bytes().data(), static_cast<size_t>(width * height * 4));

			vmaUnmapMemory(vulkan_renderer->m_allocator, staging_buffer.m_allocation);

			VkExtent3D imageExtent;
			imageExtent.width = static_cast<uint32_t>(width);
			imageExtent.height = static_cast<uint32_t>(height);
			imageExtent.depth = 1;

			VkImageCreateInfo dimg_info = image_create_info(image_format, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, imageExtent);

			AllocatedImage newImage;//create new image

			VmaAllocationCreateInfo dimg_allocinfo = {};
			dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

			//allocate and create the image
			vmaCreateImage(vulkan_renderer->m_allocator, &dimg_info, &dimg_allocinfo, &newImage._image, &newImage._allocation, nullptr);

			vulkan_renderer->immediate_submit([&](VkCommandBuffer cmd) {
				VkImageSubresourceRange range;//tell what part of the image we will transform
				range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				range.baseMipLevel = 0;
				range.levelCount = 1;
				range.baseArrayLayer = 0;
				range.layerCount = 1;

				VkImageMemoryBarrier imageBarrier_toTransfer = {};
				imageBarrier_toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

				imageBarrier_toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				imageBarrier_toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				imageBarrier_toTransfer.image = newImage._image;
				imageBarrier_toTransfer.subresourceRange = range;

				imageBarrier_toTransfer.srcAccessMask = 0;
				imageBarrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				//barrier the image into the transfer-receive layout
				vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toTransfer);

				VkBufferImageCopy copyRegion = {};
				copyRegion.bufferOffset = 0;
				copyRegion.bufferRowLength = 0;
				copyRegion.bufferImageHeight = 0;

				copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				copyRegion.imageSubresource.mipLevel = 0;
				copyRegion.imageSubresource.baseArrayLayer = 0;
				copyRegion.imageSubresource.layerCount = 1;
				copyRegion.imageExtent = imageExtent;

				//copy the buffer into the image
				vkCmdCopyBufferToImage(cmd, staging_buffer.m_buffer, newImage._image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

				//to shader readable layout
				VkImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;

				imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				//barrier the image into the shader readable layout
				vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toReadable);
			});

			vulkan_renderer->m_deletion_queue.push_function([=]() {
				vmaDestroyImage(vulkan_renderer->m_allocator, newImage._image, newImage._allocation);
			});

			//destroy staging buffer
			vmaDestroyBuffer(vulkan_renderer->m_allocator, staging_buffer.m_buffer, staging_buffer.m_allocation);
			//------------------init texture------------------

			//todo:move the initialize texture to texture class
			VkDevice device = *(VkDevice*)(vulkan_renderer->m_logic_device->get_native_handle());
			VkImageView image_view;
			VkImageViewCreateInfo imageinfo = imageview_create_info(VK_FORMAT_R8G8B8A8_SRGB, newImage._image, VK_IMAGE_ASPECT_COLOR_BIT);
			vkCreateImageView(device, &imageinfo, nullptr, &image_view);

			texture->set_image(newImage);
			texture->set_shader_resource(image_view);//todo:fix me

			m_non_atlased_textures.push_back(std::move(texture));
		}

		return new_proxy;
	}

}