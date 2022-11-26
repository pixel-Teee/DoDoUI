#pragma once

#include "Renderer/RendererInstance.h"

#include <vulkan/vulkan.h>

namespace DoDo {
	class VulkanInstance : public RendererInstance
	{
	public:
		VulkanInstance();

		virtual ~VulkanInstance();
	private:
		void setup_debug_message();

		//describe application
		VkInstance m_vulkan_instance;

		//debug messenger object, need to destory before instance
		VkDebugUtilsMessengerEXT m_debug_messenger;
	};
}