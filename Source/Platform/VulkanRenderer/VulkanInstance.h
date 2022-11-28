#pragma once

#include "Renderer/RendererInstance.h"

#include <vulkan/vulkan.h>

namespace DoDo {
	class Device;
	class SwapChain;
	class Shader;
	class PipelineStateObject;
	class VulkanInstance : public RendererInstance
	{
	public:
		VulkanInstance(Window& window);

		virtual ~VulkanInstance();

		virtual void Destroy() override;

		virtual void wait_for_idle() override;

		//------test------
		virtual void draw_frame() override;
		//------test------
	private:
		void setup_debug_message();

		void pick_physical_device();

		bool check_device_extension_support(VkPhysicalDevice device);
		bool is_device_suitable(VkPhysicalDevice device);

		void create_surface(Window& window);

		//------test------
		void create_command_pool();
		VkCommandPool m_command_pool;
		void create_command_buffer();
		VkCommandBuffer m_command_buffer;
		void record_command_buffer(uint32_t image_index);
		void create_sync_objects();
		//------test------

		//describe application
		VkInstance m_vulkan_instance;

		//debug messenger object, need to destory before instance
		VkDebugUtilsMessengerEXT m_debug_messenger;

		//adapter
		VkPhysicalDevice m_physical_device;

		//surface
		VkSurfaceKHR m_surface;

		Scope<Device> m_p_logic_device;

		Scope<SwapChain> m_p_swap_chain;

		//------test------
		Scope<Shader> m_vertex_shader_module;
		Scope<Shader> m_fragment_shader_module;
		Scope<PipelineStateObject> m_pipeline_state_object;

		//VkCommandPool m_command_pool;

		VkSemaphore m_image_available_semaphore;
		VkSemaphore m_render_finished_semaphore;
		VkFence m_flight_fence;
		//------test------
	};
}