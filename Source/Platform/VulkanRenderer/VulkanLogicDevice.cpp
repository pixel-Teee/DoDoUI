#include <PreCompileHeader.h>

#include "VulkanLogicDevice.h"

#include <vulkan/vulkan.h>

#include "Utils.h"

namespace DoDo {
#ifdef DEBUG
	static constexpr bool enable_validation_layers = true;
#else
	static constexpr bool enable_validation_layers = false;
#endif

	static const std::vector<const char*> device_extensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	static const std::vector<const char*> validation_layers = {
		"VK_LAYER_KHRONOS_validation"
	};

	VulkanLogicDevice::VulkanLogicDevice(void* adapter, void* surface)
	{

		VkPhysicalDevice device = *(VkPhysicalDevice*)adapter;

		VkSurfaceKHR surface_khr = *(VkSurfaceKHR*)surface;

		VulkanUtils::QueueFamilyIndices indices = VulkanUtils::find_queue_families(device, surface_khr);

		//------queue create information------
		std::vector<VkDeviceQueueCreateInfo> queue_create_infos{};
		std::set<uint32_t> unique_queue_families = { indices.graphics_family.value(), indices.present_family.value() };
		float queue_priority = 1.0f;
		for (uint32_t queue_family : unique_queue_families) {
			VkDeviceQueueCreateInfo queue_create_info{};
			queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_create_info.queueFamilyIndex = queue_family;
			queue_create_info.queueCount = 1;
			queue_create_info.pQueuePriorities = &queue_priority;
			queue_create_infos.push_back(queue_create_info);
		}
		//------queue create information------

		VkPhysicalDeviceFeatures device_features{};
		device_features.samplerAnisotropy = VK_TRUE;

		//------logic device create information------
		VkDeviceCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
		create_info.pQueueCreateInfos = queue_create_infos.data();

		create_info.pEnabledFeatures = &device_features;

		create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
		create_info.ppEnabledExtensionNames = device_extensions.data();
		//------logic device create information------

		if (enable_validation_layers)
		{
			create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
			create_info.ppEnabledLayerNames = validation_layers.data();
		}
		else
		{
			create_info.enabledLayerCount = 0;
		}

		if (vkCreateDevice(device, &create_info, nullptr, &m_logic_device) != VK_SUCCESS) {
			//throw std::runtime_error("failed to create logical device!");
			std::cout << "failed to create logical device!" << std::endl;
		}
	}
	VulkanLogicDevice::~VulkanLogicDevice()
	{
		//vkDestroyDevice(m_logic_device, nullptr);
		//vkDestroyDevice(m_logic_device, nullptr);
	}

	void VulkanLogicDevice::destroy()
	{
		vkDestroyDevice(m_logic_device, nullptr);
	}

	void* VulkanLogicDevice::get_native_handle()
	{
		return &m_logic_device;
	}

	//void VulkanLogicDevice::create_surface()
	//{
	//	VkWin32SurfaceCreateInfoKHR create_info{};
	//	create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	//	//create_info.hwnd = ;
	//	create_info.hwnd = 
	//}

}