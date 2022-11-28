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

		virtual void* get_native_handle() override;

		virtual void* get_graphics_queue() override;

		virtual void* get_present_queue() override;

		//QueueFamilyIndices find_queue_families(VkPhysicalDevice device);
	private:
		//void create_surface();

		VkDevice m_logic_device;

		VkQueue m_graphics_queue;

		VkQueue m_present_queue;
	};
}