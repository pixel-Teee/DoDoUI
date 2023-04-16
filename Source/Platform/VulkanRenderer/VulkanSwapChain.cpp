#include <PreCompileHeader.h>

#include "VulkanSwapChain.h"

#include "SlateVulkanRenderer.h"
#include "Utils.h"

#include "Core/Window.h"

#ifdef WIN32
    //------vulkan for glfw------
    #define VK_USE_PLATFORM_WIN32_KHR
    #define GLFW_INCLUDE_VULKAN
    #include <GLFW/glfw3.h>
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h>
    #include <vulkan/vulkan_win32.h>
    //------vulkan for glfw------
#elif defined Linux
    //------vulkan for glfw------
    #define VK_USE_PLATFORM_XLIB_KHR
    #define GLFW_INCLUDE_VULKAN
    #include <GLFW/glfw3.h>
    #define GLFW_EXPOSE_NATIVE_X11
    #include <GLFW/glfw3native.h>
    #include <vulkan/vulkan_xlib.h>
    //------vulkan for glfw------
#elif defined Android
	#include <android_native_app_glue.h>
#endif

namespace DoDo {
	static VulkanUtils::SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice vulkan_physical_device, VkSurfaceKHR surface)
	{
		VulkanUtils::SwapChainSupportDetails swap_chain_support_details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkan_physical_device, surface, &swap_chain_support_details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_physical_device, surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			swap_chain_support_details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_physical_device, surface, &formatCount, swap_chain_support_details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan_physical_device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			swap_chain_support_details.present_modes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan_physical_device, surface, &presentModeCount, swap_chain_support_details.present_modes.data());
		}

		return swap_chain_support_details;
	}

	static VkSurfaceFormatKHR choose_swap_surface_format(std::vector<VkSurfaceFormatKHR> available_formats)
	{
		for (const auto& availableFormat : available_formats) {
			if (availableFormat.format == VK_FORMAT_R8G8B8A8_UNORM)
				return availableFormat;
		}

		return available_formats[0];
	}

	static VkPresentModeKHR choose_swap_present_mode(std::vector<VkPresentModeKHR> present_modes)
	{
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	static VkExtent2D choose_swap_extent(VkSurfaceCapabilitiesKHR capabilities_khr, Window& window)
	{
		if (capabilities_khr.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) {
			return capabilities_khr.currentExtent;
		}
		else
		{
#ifndef Android
			int width, height;
			glfwGetFramebufferSize((GLFWwindow*)window.get_window_native_handle(), &width, &height);
#else
			int width, height;
			width = capabilities_khr.currentExtent.width;
			height = capabilities_khr.currentExtent.height;
#endif
			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};


			actualExtent.width = std::clamp(actualExtent.width, capabilities_khr.minImageExtent.width, capabilities_khr.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities_khr.minImageExtent.height, capabilities_khr.maxImageExtent.height);

			return actualExtent;
		}
	}

	VulkanSwapChain::VulkanSwapChain(void* vulkan_physical_device, void* logic_device, void* surface, void* render_pass, Window& window)
	{
		VkPhysicalDevice physical_device = *(VkPhysicalDevice*)vulkan_physical_device;

		VkDevice device = *(VkDevice*)logic_device;

		VkSurfaceKHR surface_khr = *(VkSurfaceKHR*)surface;

		VkRenderPass pass = *(VkRenderPass*)render_pass;

		VulkanUtils::SwapChainSupportDetails swap_chain_support_details;

		swap_chain_support_details = query_swap_chain_support(physical_device, surface_khr);

		m_surface_format = choose_swap_surface_format(swap_chain_support_details.formats);

		VkPresentModeKHR present_mode = choose_swap_present_mode(swap_chain_support_details.present_modes);

		m_extent_2d = choose_swap_extent(swap_chain_support_details.capabilities, window);

		//------get image count(image just buffer)------
		uint32_t image_count = swap_chain_support_details.capabilities.minImageCount + 1;

		if (swap_chain_support_details.capabilities.maxImageCount > 0 && image_count > swap_chain_support_details.capabilities.maxImageCount)
		{
			image_count = swap_chain_support_details.capabilities.maxImageCount;
		}
		//------get image count(image just buffer)------

		VkSwapchainCreateInfoKHR create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = surface_khr;

		create_info.minImageCount = image_count;
		create_info.imageFormat = m_surface_format.format;
		create_info.imageColorSpace = m_surface_format.colorSpace;
		create_info.imageExtent = m_extent_2d;
		create_info.imageArrayLayers = 1;//describe every image consist of array layer numbers
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;//as attachment 

		VulkanUtils::QueueFamilyIndices indices = VulkanUtils::find_queue_families(physical_device, surface_khr);

		uint32_t queue_family_indices[] = { indices.graphics_family.value(), indices.present_family.value() };

		if (indices.graphics_family != indices.present_family)
		{
			create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			create_info.queueFamilyIndexCount = 2;
			create_info.pQueueFamilyIndices = queue_family_indices;
		}
		else
		{
			create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			create_info.queueFamilyIndexCount = 0;//optional
			create_info.pQueueFamilyIndices = nullptr;//optional
		}

		create_info.preTransform = swap_chain_support_details.capabilities.currentTransform;
		
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		create_info.clipped = VK_TRUE;

		create_info.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(device, &create_info, nullptr, &m_swap_chain) != VK_SUCCESS)
		{
			std::cout << "failed to create device" << std::endl;
		}

		//------create swap chain------
		vkGetSwapchainImagesKHR(device, m_swap_chain, &image_count, nullptr);
		m_swap_chain_images.resize(image_count);
		vkGetSwapchainImagesKHR(device, m_swap_chain, &image_count, m_swap_chain_images.data());
		//------create swap chain------

		//create_image_views(device);
		//
		//create_frame_buffers(device, pass);
	}

	VulkanSwapChain::VulkanSwapChain(void* vulkan_physical_device, void* logic_device, void* surface, Window& window, DeletionQueue& deletion_queue)
	{
		VkPhysicalDevice physical_device = *(VkPhysicalDevice*)vulkan_physical_device;

		VkDevice device = *(VkDevice*)logic_device;

		VkSurfaceKHR surface_khr = *(VkSurfaceKHR*)surface;

		//VkRenderPass pass = *(VkRenderPass*)render_pass;

		VulkanUtils::SwapChainSupportDetails swap_chain_support_details;

		swap_chain_support_details = query_swap_chain_support(physical_device, surface_khr);

		m_surface_format = choose_swap_surface_format(swap_chain_support_details.formats);

		VkPresentModeKHR present_mode = choose_swap_present_mode(swap_chain_support_details.present_modes);

		m_extent_2d = choose_swap_extent(swap_chain_support_details.capabilities, window);

		//------get image count(image just buffer)------
		uint32_t image_count = swap_chain_support_details.capabilities.minImageCount + 1;

		if (swap_chain_support_details.capabilities.maxImageCount > 0 && image_count > swap_chain_support_details.capabilities.maxImageCount)
		{
			image_count = swap_chain_support_details.capabilities.maxImageCount;
		}
		//------get image count(image just buffer)------

		VkSwapchainCreateInfoKHR create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = surface_khr;

		create_info.minImageCount = image_count;
		create_info.imageFormat = m_surface_format.format;
		create_info.imageColorSpace = m_surface_format.colorSpace;
		create_info.imageExtent = m_extent_2d;
		create_info.imageArrayLayers = 1;//describe every image consist of array layer numbers
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;//as attachment 

		VulkanUtils::QueueFamilyIndices indices = VulkanUtils::find_queue_families(physical_device, surface_khr);

		uint32_t queue_family_indices[] = { indices.graphics_family.value(), indices.present_family.value() };

		if (indices.graphics_family != indices.present_family)
		{
			create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			create_info.queueFamilyIndexCount = 2;
			create_info.pQueueFamilyIndices = queue_family_indices;
		}
		else
		{
			create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			create_info.queueFamilyIndexCount = 0;//optional
			create_info.pQueueFamilyIndices = nullptr;//optional
		}

		create_info.preTransform = swap_chain_support_details.capabilities.currentTransform;

		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		create_info.clipped = VK_TRUE;

		create_info.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(device, &create_info, nullptr, &m_swap_chain) != VK_SUCCESS)
		{
			std::cout << "failed to create swap chain" << std::endl;
		}

		//------create swap chain------
		vkGetSwapchainImagesKHR(device, m_swap_chain, &image_count, nullptr);
		m_swap_chain_images.resize(image_count);
		vkGetSwapchainImagesKHR(device, m_swap_chain, &image_count, m_swap_chain_images.data());
		//------create swap chain------

		deletion_queue.push_function([=]()
		{
			vkDestroySwapchainKHR(device, m_swap_chain, nullptr);
		});

		//todo:implement create_image_views
		create_image_views(device, deletion_queue);

		//create_frame_buffers(device, pass);

		//todo:add render pass for create framebuffer
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		//vkDestroySwapchainKHR(device, swapChain, nullptr);
	}
	void VulkanSwapChain::Destroy(void* logic_device)
	{
		VkDevice device = *(VkDevice*)logic_device;

		for (size_t i = 0; i < m_swap_chain_frame_buffers.size(); ++i)
		{
			vkDestroyFramebuffer(device, m_swap_chain_frame_buffers[i], nullptr);
		}
		
		for (size_t i = 0; i < m_swap_chain_image_views.size(); ++i)
		{
			vkDestroyImageView(device, m_swap_chain_image_views[i], nullptr);
		}

		vkDestroySwapchainKHR(device, m_swap_chain, nullptr);
	}

	void* VulkanSwapChain::get_native_handle()
	{
		return &m_swap_chain;
	}

	void* VulkanSwapChain::get_framebuffer(uint32_t index)
	{
		//VkFramebuffer frame_buffer = m_swap_chain_frame_buffers[index];
		//frame_index = (frame_index + 1) % 2;
		return &m_swap_chain_frame_buffers[index];
	}

	std::pair<uint32_t, uint32_t> VulkanSwapChain::get_swap_chain_extent()
	{
		return std::make_pair(m_extent_2d.width, m_extent_2d.height);
	}

	std::vector<VkFramebuffer> VulkanSwapChain::create_frame_buffer(VkDevice logic_device, VkRenderPass render_pass, DeletionQueue& deletion_queue)
	{
		std::vector<VkFramebuffer> framebuffers;
		//render pass describes how to control image and how to clear image
		framebuffers.resize(m_swap_chain_image_views.size());

		for (size_t i = 0; i < framebuffers.size(); ++i)
		{
			VkImageView attachments[] = { m_swap_chain_image_views[i] };

			VkFramebufferCreateInfo frame_buffer_info{};
			frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			frame_buffer_info.renderPass = render_pass;//render pass;
			frame_buffer_info.attachmentCount = 1;
			frame_buffer_info.pAttachments = attachments;
			frame_buffer_info.width = m_extent_2d.width;
			frame_buffer_info.height = m_extent_2d.height;
			frame_buffer_info.layers = 1;

			if (vkCreateFramebuffer(logic_device, &frame_buffer_info, nullptr, &framebuffers[i]) != VK_SUCCESS)
			{
				std::cout << "failed to create framebuffer" << std::endl;
			}
		}

		for (size_t i = 0; i < framebuffers.size(); ++i)
		{
			deletion_queue.push_function([=]()
			{
				vkDestroyFramebuffer(logic_device, framebuffers[i], nullptr);
			});
		}

		//copy return
		return framebuffers;
	}

	void VulkanSwapChain::create_image_views(VkDevice logic_device)
	{
		m_swap_chain_image_views.resize(m_swap_chain_images.size());

		for (size_t i = 0; i < m_swap_chain_images.size(); ++i)
		{
			m_swap_chain_image_views[i] = create_image_view(m_swap_chain_images[i], m_surface_format.format, VK_IMAGE_ASPECT_COLOR_BIT, logic_device);
		}
	}

	void VulkanSwapChain::create_image_views(VkDevice logic_device, DeletionQueue& deletion_queue)
	{
		m_swap_chain_image_views.resize(m_swap_chain_images.size());

		for (size_t i = 0; i < m_swap_chain_images.size(); ++i)
		{
			m_swap_chain_image_views[i] = create_image_view(m_swap_chain_images[i], m_surface_format.format, VK_IMAGE_ASPECT_COLOR_BIT, logic_device, deletion_queue);
		}
	}

	void VulkanSwapChain::create_frame_buffers(VkDevice logic_device, VkRenderPass render_pass)
	{
		//render pass describes how to control image and how to clear image
		m_swap_chain_frame_buffers.resize(m_swap_chain_image_views.size());

		for (size_t i = 0; i < m_swap_chain_frame_buffers.size(); ++i)
		{
			VkImageView attachments[] = { m_swap_chain_image_views[i] };

			VkFramebufferCreateInfo frame_buffer_info{};
			frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			frame_buffer_info.renderPass = render_pass;//render pass;
			frame_buffer_info.attachmentCount = 1;
			frame_buffer_info.pAttachments = attachments;
			frame_buffer_info.width = m_extent_2d.width;
			frame_buffer_info.height = m_extent_2d.height;
			frame_buffer_info.layers = 1;

			if (vkCreateFramebuffer(logic_device, &frame_buffer_info, nullptr, &m_swap_chain_frame_buffers[i]) != VK_SUCCESS)
			{
				std::cout << "failed to create framebuffer" << std::endl;
			}
		}
	}

	VkImageView VulkanSwapChain::create_image_view(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags, VkDevice logic_device)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspect_flags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		if (vkCreateImageView(logic_device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}

		return imageView;
	}

	VkImageView VulkanSwapChain::create_image_view(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags,
		VkDevice logic_device, DeletionQueue& deletion_queue)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspect_flags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		if (vkCreateImageView(logic_device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}

		deletion_queue.push_function([=]()
		{
			vkDestroyImageView(logic_device, imageView, nullptr);
		});

		return imageView;
	}
}
