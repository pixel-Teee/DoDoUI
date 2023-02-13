#include <PreCompileHeader.h>

#include "SwapChain.h"

#include "Platform/VulkanRenderer/VulkanSwapChain.h"

namespace DoDo {
	SwapChain::~SwapChain()
	{
	}
	Scope<SwapChain> SwapChain::Create(void* vulkan_physical_device, void* logic_device, void* surface, void* render_pass, Window& window)
	{
		return CreateScope<VulkanSwapChain>(vulkan_physical_device, logic_device, surface, render_pass, window);
	}

	std::shared_ptr<SwapChain> SwapChain::Create(void* vulkan_physical_device, void* logic_device, void* surface,
		Window& window, DeletionQueue& queue)
	{
		return std::make_shared<VulkanSwapChain>(vulkan_physical_device, logic_device, surface, window, queue);
	}
}
