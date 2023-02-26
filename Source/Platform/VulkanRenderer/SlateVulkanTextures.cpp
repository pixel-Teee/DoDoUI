#include <PreCompileHeader.h>

#include "SlateVulkanTextures.h"

#include "Core/Math/IntRect.h"

#include "Application/Application.h"

#include "Platform/VulkanRenderer/SlateVulkanRenderer.h"

#include "Platform/VulkanRenderer/VulkanLogicDevice.h"

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
		update_texture_raw(bytes.data(), FIntRect());
	}

	void FSlateVulkanTexture::init(VkFormat in_format, void* initial_data, bool b_use_staging_texture)
	{
		//------init texture------
		Renderer* renderer = Application::get().get_renderer();
		FSlateVulkanRenderer* vulkan_renderer = (FSlateVulkanRenderer*)(renderer);

		m_bytes_per_pixel = in_format == VK_FORMAT_R8_UNORM ? 1 : 4;

		//allocate temporary buffer for holding texture data to upload
		m_staging_buffer = vulkan_renderer->create_buffer(m_size_x * m_size_y * m_bytes_per_pixel, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

		//copy data to buffer
		void* data;
		vmaMapMemory(vulkan_renderer->m_allocator, m_staging_buffer.m_allocation, &data);

		if (initial_data)
			memcpy(data, initial_data, static_cast<size_t>(m_size_x * m_size_y * m_bytes_per_pixel));
		else
			memset(data, 0, static_cast<size_t>(m_size_x * m_size_y * m_bytes_per_pixel));//note:reset 0

		vmaUnmapMemory(vulkan_renderer->m_allocator, m_staging_buffer.m_allocation);

		VkExtent3D imageExtent;
		imageExtent.width = static_cast<uint32_t>(m_size_x);
		imageExtent.height = static_cast<uint32_t>(m_size_y);
		imageExtent.depth = 1;

		VkImageCreateInfo dimg_info = image_create_info(in_format, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, imageExtent);

		AllocatedImage new_image;//create new image

		VmaAllocationCreateInfo dimg_allocinfo = {};

		dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		//allocate and create the image
		vmaCreateImage(vulkan_renderer->m_allocator, &dimg_info, &dimg_allocinfo, &new_image._image, &new_image._allocation, nullptr);

		//transfer operation
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
			imageBarrier_toTransfer.image = new_image._image;
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
			copyRegion.imageExtent = imageExtent;//copy region

			//copy the buffer into the image
			vkCmdCopyBufferToImage(cmd, m_staging_buffer.m_buffer, new_image._image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
			
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
			vmaDestroyBuffer(vulkan_renderer->m_allocator, m_staging_buffer.m_buffer, m_staging_buffer.m_allocation);
			vmaDestroyImage(vulkan_renderer->m_allocator, new_image._image, new_image._allocation);
		});
		
		//------init texture------

		//------create image view------
		VkDevice device = *(VkDevice*)(vulkan_renderer->m_logic_device->get_native_handle());

		VkImageView image_view;

		VkImageViewCreateInfo image_info = imageview_create_info(in_format, new_image._image, VK_IMAGE_ASPECT_COLOR_BIT);

		vkCreateImageView(device, &image_info, nullptr, &image_view);

		vulkan_renderer->m_deletion_queue.push_function([=]() {
			vkDestroyImageView(device, image_view, nullptr);
		});

		set_image(new_image);
		set_shader_resource(image_view);
		//------create image view------
	}

	void FSlateVulkanTexture::update_texture_raw(const void* buffer, const FIntRect& dirty)
	{
		bool b_use_staging_texture = true;

		int32_t left = 0;
		int32_t top = 0;
		int32_t right = 0;
		int32_t bottom = 0;

		if (b_use_staging_texture && dirty.Area() > 0)
		{
			left = dirty.m_min.x;
			top = dirty.m_min.y;
			right = dirty.m_max.x;
			bottom = dirty.m_max.y;
		}
		else
		{
			left = 0;
			right = m_size_x;
			top = 0;
			bottom = m_size_y;
		}

		Renderer* renderer = Application::get().get_renderer();
		FSlateVulkanRenderer* vulkan_renderer = (FSlateVulkanRenderer*)(renderer);

		//------update texture------
		void* data;
		vmaMapMemory(vulkan_renderer->m_allocator, m_staging_buffer.m_allocation, &data);

		uint32_t source_pitch = m_size_x * m_bytes_per_pixel;//row size
		uint32_t copy_row_bytes = (right - left) * m_bytes_per_pixel;
		uint8_t* destination = (uint8_t*)data + left * m_bytes_per_pixel + top * source_pitch;
		uint8_t* source = (uint8_t*)buffer + left * m_bytes_per_pixel + top * source_pitch;

		for (uint32_t row = top; row < bottom; ++row, destination += source_pitch, source += source_pitch)
		{
			std::memcpy(destination, source, copy_row_bytes);
		}

		vmaUnmapMemory(vulkan_renderer->m_allocator, m_staging_buffer.m_allocation);
		//------update texture------

		VkExtent3D imageExtent;
		imageExtent.width = static_cast<uint32_t>(m_size_x);
		imageExtent.height = static_cast<uint32_t>(m_size_y);
		imageExtent.depth = 1;

		vulkan_renderer->immediate_submit([&](VkCommandBuffer cmd) {
			VkImageSubresourceRange range;//tell what part of the image we will transform
			range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			range.baseMipLevel = 0;
			range.levelCount = 1;
			range.baseArrayLayer = 0;
			range.layerCount = 1;

			VkImageMemoryBarrier imageBarrier_toTransfer = {};
			imageBarrier_toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

			imageBarrier_toTransfer.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageBarrier_toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageBarrier_toTransfer.image = m_image._image;
			imageBarrier_toTransfer.subresourceRange = range;

			imageBarrier_toTransfer.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			imageBarrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			//barrier the image into the transfer-receive layout
			vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toTransfer);

			VkBufferImageCopy copyRegion = {};
			copyRegion.bufferOffset = 0;
			copyRegion.bufferRowLength = 0;
			copyRegion.bufferImageHeight = 0;

			copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.imageSubresource.mipLevel = 0;
			copyRegion.imageSubresource.baseArrayLayer = 0;
			copyRegion.imageSubresource.layerCount = 1;
			copyRegion.imageExtent = imageExtent;//copy region

			//copy the buffer into the image
			vkCmdCopyBufferToImage(cmd, m_staging_buffer.m_buffer, m_image._image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

			//to shader readable layout
			VkImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;

			imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			//barrier the image into the shader readable layout
			vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toReadable);
		});
	}

	//void FSlateVulkanTexture::set_descriptor_set(VkDescriptorSet descriptor_set)
	//{
	//	m_descriptor_set = descriptor_set;
	//}
	uint32_t FSlateVulkanTexture::get_width() const
	{
		return m_size_x;
	}
	uint32_t FSlateVulkanTexture::get_height() const
	{
		return m_size_y;
	}
	FSlateFontAtlasVulkan::FSlateFontAtlasVulkan(uint32_t width, uint32_t height, const bool in_is_gray_scale)
		: FSlateFontAtlas(width, height, in_is_gray_scale)
	{
		//todo:create a font texture atlas
		m_font_texture = new FSlateVulkanTexture(width, height);//holds the vulkan texture life time
		m_font_texture->init(VK_FORMAT_R8_UNORM, nullptr, true);
		//todo:implement init function
	}
	FSlateFontAtlasVulkan::~FSlateFontAtlasVulkan()
	{
		delete m_font_texture;
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
