#pragma once

#include "Renderer/Device.h"

#include <vulkan/vulkan.h>

namespace DoDo {

	class VulkanLogicDevice : public Device
	{
	public:
		VulkanLogicDevice(void* adapter, void* surface);

		virtual ~VulkanLogicDevice();

		virtual void destroy() override;

		//QueueFamilyIndices find_queue_families(VkPhysicalDevice device);
	private:
		//void create_surface();

		VkDevice m_logic_device;
	};
}