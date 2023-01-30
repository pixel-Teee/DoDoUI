#pragma once

#include "glm/glm.hpp"

#include <Vulkan/vulkan.h>

#include "Core/Core.h"

#include "SlateCore/Rendering/SlateDrawBuffer.h"

#include "Renderer/Renderer.h"//slate renderer

#include <queue>
#include <functional>

namespace DoDo {
	struct DeletionQueue
	{
		std::deque<std::function<void()>> deletors;

		void push_function(std::function<void()>&& function) {
			deletors.push_back(function);
		}

		void flush() {
			// reverse iterate the deletion queue to execute all the functions
			for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
				(*it)(); //call the function
			}

			deletors.clear();
		}
	};

	class VulkanSwapChain;

	struct FSlateVulkanViewport
	{
		glm::mat4 m_projection_matrix;
		
		VkViewport m_view_port_info;

		std::shared_ptr<VulkanSwapChain> m_vulkan_swap_chain;

		VkSurfaceKHR m_vulkan_surface;//every window have this

		std::vector<VkFramebuffer> m_vulkan_framebuffer;//framebuffer connect the render pass and vk image(will be render to)

		VkSemaphore m_present_semaphore;

		VkSemaphore m_render_semaphore;

		VkFence m_fence;

		VkCommandBuffer m_command_buffer;

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
	class Device;
	class Window;//platform window
	class FSlateVulkanRenderer : public Renderer//todo:need to inherited from FSlateRenderer
	{
	public:
		virtual ~FSlateVulkanRenderer();

		/*FSlateRenderer Interface*/
		//virtual bool Initialize();
		FSlateDrawBuffer& get_draw_buffer() override;

		void create_view_port(const std::shared_ptr<SWindow> in_window) override;

		void draw_windows(FSlateDrawBuffer& in_window_draw_buffer) override;

		bool initialize() override;

		virtual void destroy() override;
		/*FSlateRenderer Interface*/

		bool create_device();
	private:
		void private_create_view_port(std::shared_ptr<SWindow> in_window, glm::vec2& window_size);

		void pick_physical_device();

		void setup_debug_message();

		void create_surface(Window& window, VkSurfaceKHR& surface);

		void create_sync_objects(FSlateVulkanViewport& view_port);

		void create_command_pool();

		void create_command_buffer(FSlateVulkanViewport& view_port);

		void init_frame_buffers();

		void init_default_render_pass();
	private:
		bool m_b_has_attempted_initialization;

		glm::mat4 m_view_matrix;

		std::map<const SWindow*, FSlateVulkanViewport> m_window_to_viewport_map;

		FSlateDrawBuffer m_draw_buffer;

		//todo:implement FSlateElementBatcher
		//todo:implement FSlateD3DTextureManager
		//todo:implement FSlateD3D11RenderingPolicy

		//debug messenger object, need to destory before instance
		VkDebugUtilsMessengerEXT m_debug_messenger;

		//vulkan context
		VkInstance m_vulkan_instance;//todo:in the future, move this to other place

		VkPhysicalDevice m_physical_device;//gpu

		DeletionQueue m_deletion_queue;//deletion queue

		std::shared_ptr<Device> m_logic_device;//logic device

		VkFence m_render_fence;//renderer fence

		VkSemaphore m_present_semaphore;
		VkSemaphore m_render_semaphore;

		VkRenderPass m_render_pass;

		VkCommandPool m_command_pool;

		uint64_t m_frame_number;

		//std::vector<VkFramebuffer> m_frame_buffer;//frame buffer connect the render pass and vk image
	};

}
