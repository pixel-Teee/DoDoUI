#pragma once

#include "Core/Core.h"

namespace DoDo {
	class Window;
	class SwapChain {
	public:
		virtual ~SwapChain();

		virtual void Destroy(void* logic_device) = 0;

		static Scope<SwapChain> Create(void* vulkan_physical_device, void* logic_device, void* surface, Window& window);
	};
}