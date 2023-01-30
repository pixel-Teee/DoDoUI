#pragma once

#include "Renderer/SwapChain.h"

#include <vulkan/vulkan.h>

namespace DoDo {
	class VulkanSwapChain : public SwapChain
	{
	public:
		VulkanSwapChain(void* vulkan_physical_device, void* logic_device, void* surface, void* render_pass, Window& window);

		VulkanSwapChain(void* vulkan_physical_device, void* logic_device, void* surface, Window& window, DeletionQueue& deletion_queue);

		virtual ~VulkanSwapChain();

		virtual void Destroy(void* logic_device) override;

		virtual void* get_native_handle() override;

		virtual void* get_framebuffer(uint32_t index) override;

		virtual std::pair<uint32_t, uint32_t> get_swap_chain_extent() override;

		//framebuffer connect the render pass to the VkImage rendering to
		std::vector<VkFramebuffer> create_frame_buffer(VkDevice logic_device, VkRenderPass render_pass, DeletionQueue& deletion_queue);

	private:
		void create_image_views(VkDevice logic_device);

		void create_image_views(VkDevice logic_device, DeletionQueue& deletion_queue);

		void create_frame_buffers(VkDevice logic_device, VkRenderPass render_pass);

		VkImageView create_image_view(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags, VkDevice logic_device);

		VkImageView create_image_view(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags, VkDevice logic_device, DeletionQueue& deletion_queue);

		VkSwapchainKHR m_swap_chain;

		VkSurfaceFormatKHR m_surface_format;

		//VkPresentModeKHR m_present_mode;

		//image is buffer
		std::vector<VkImage> m_swap_chain_images;

		std::vector<VkImageView> m_swap_chain_image_views;

		std::vector<VkFramebuffer> m_swap_chain_frame_buffers;

		VkExtent2D m_extent_2d;

		//current frame index
		uint32_t frame_index = 0;
	};
}