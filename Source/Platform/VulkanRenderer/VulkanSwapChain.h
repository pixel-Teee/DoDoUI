#pragma once

#include "Renderer/SwapChain.h"

#include <vulkan/vulkan.h>

namespace DoDo {
	class VulkanSwapChain : public SwapChain
	{
	public:
		VulkanSwapChain(void* vulkan_physical_device, void* logic_device, void* surface, Window& window);

		virtual ~VulkanSwapChain();

		virtual void Destroy(void* logic_device) override;

	private:
		void create_image_views(VkDevice logic_device);

		VkImageView create_image_view(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags, VkDevice logic_device);

		VkSwapchainKHR m_swap_chain;

		VkSurfaceFormatKHR m_surface_format;

		//VkPresentModeKHR m_present_mode;

		//image is buffer
		std::vector<VkImage> m_swap_chain_images;

		std::vector<VkImageView> m_swap_chain_image_views;

		VkExtent2D m_extent_2d;
	};
}