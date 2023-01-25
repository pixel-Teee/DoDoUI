#pragma once

#include "glm/glm.hpp"

#include <Vulkan/vulkan.h>

#include "Core/Core.h"

#include "SlateCore/Rendering/SlateDrawBuffer.h"

namespace DoDo {
	class VulkanSwapChain;

	struct FSlateVulkanViewport
	{
		glm::mat4 m_projection_matrix;
		
		VkViewport m_view_port_info;

		std::shared_ptr<VulkanSwapChain> m_vulkan_swap_chain;

		//todo:implement render target and render target view

		bool m_b_full_screen;

		FSlateVulkanViewport()
			: m_b_full_screen(false)
		{

		}

		~FSlateVulkanViewport()
		{
			//destroy render target
		}
	};

	class SWindow;//forward declare

	class FSlateVulkanRenderer//todo:need to inherited from FSlateRenderer
	{
	public:
		~FSlateVulkanRenderer();

		/*FSlateRenderer Interface*/
		virtual bool Initialize();
		/*FSlateRenderer Interface*/
	private:
		bool m_b_has_attempted_initialization;

		glm::mat4 m_view_matrix;

		std::map<const SWindow*, FSlateVulkanViewport> m_window_to_viewport_map;

		FSlateDrawBuffer m_draw_buffer;

		//todo:implement FSlateElementBatcher
		//todo:implement FSlateD3DTextureManager
		//todo:implement FSlateD3D11RenderingPolicy
	};
}