#pragma once

#include "Core/Core.h"

namespace DoDo {
	class Window;
	struct DeletionQueue;
	class SwapChain {
	public:
		virtual ~SwapChain();

		virtual void Destroy(void* logic_device) = 0;

		virtual void* get_framebuffer(uint32_t index) = 0;

		virtual void* get_native_handle() = 0;

		virtual std::pair<uint32_t, uint32_t> get_swap_chain_extent() = 0;

		static Scope<SwapChain> Create(void* vulkan_physical_device, void* logic_device, void* surface, void* render_pass, Window& window);

		//todo:add render pass for create framebuffer
		static std::shared_ptr<SwapChain> Create(void* vulkan_physical_device, void* logic_device, void* surface, Window& window, DeletionQueue& queue);
	};
}