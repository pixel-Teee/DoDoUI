#include "Device.h"

#include "Platform/VulkanRenderer/VulkanLogicDevice.h"

namespace DoDo {
	Device::~Device()
	{
	}
	Scope<Device> Device::CreateDevice(void* adapter, void* surface)
	{
		return CreateScope<VulkanLogicDevice>(adapter, surface);
	}

	std::shared_ptr<Device> Device::create(void* adapter, DeletionQueue& deletion_queue)
	{
		return std::make_shared<VulkanLogicDevice>(adapter, deletion_queue);
	}
}
