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
}
