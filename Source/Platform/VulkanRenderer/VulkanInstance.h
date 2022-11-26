#pragma once

#include "Renderer/RendererInstance.h"

#include <vulkan/vulkan.h>

namespace DoDo {
	class Device;
	class SwapChain;
	class VulkanInstance : public RendererInstance
	{
	public:
		VulkanInstance(Window& window);

		virtual ~VulkanInstance();

		virtual void Destroy() override;
	private:
		void setup_debug_message();

		void pick_physical_device();

		bool check_device_extension_support(VkPhysicalDevice device);
		bool is_device_suitable(VkPhysicalDevice device);

		void create_surface(Window& window);

		//describe application
		VkInstance m_vulkan_instance;

		//debug messenger object, need to destory before instance
		VkDebugUtilsMessengerEXT m_debug_messenger;

		//adapter
		VkPhysicalDevice m_physical_device;

		//surface
		VkSurfaceKHR m_surface;

		Scope<Device> m_p_logic_device;

		Scope<SwapChain> m_p_swap_chain;
	};
}