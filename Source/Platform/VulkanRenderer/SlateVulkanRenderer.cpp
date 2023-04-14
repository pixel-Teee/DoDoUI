#include <PreCompileHeader.h>

#include "SlateVulkanRenderer.h"

//#include <include/vk_mem_alloc.h>//todo:need to fix this prefix

#include "SlateCore/Widgets/SWindow.h"

#include "VulkanLogicDevice.h"

#include "Utils.h"
#include "VulkanSwapChain.h"
#include "Core/Window.h"//platform window
#include "Renderer/SwapChain.h"//swap chain
#include "SlateCore/Rendering/DrawElements.h"
#include "SlateCore/Rendering/ElementBatcher.h"
//#include "Renderer/Device.h"

#include "SlateVulkanRenderingPolicy.h"//rendering policy

#include "SlateVulkanTextureManager.h"

#include "Core/Misc/Paths.h"//shader path depends on it

#include "VulkanInitializers.h"

#include "SlateCore/Fonts/FontTypes.h"

#include "SlateCore/Fonts/FontCache.h"//FSlateFontCache depends on it

#ifdef WIN32
//------vulkan for glfw------
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan_win32.h>
//------vulkan for glfw------
#elif defined Linux
//------vulkan for glfw------
#define VK_USE_PLATFORM_XLIB_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan_xlib.h>
//------vulkan for glfw------
#elif defined Android
#include <android_native_app_glue.h>
#include <vulkan/vulkan_android.h>
#endif

namespace DoDo {
#define VK_CHECK(x) \
	do \
	{ \
		VkResult err = x;\
		if(err)\
		{\
			std::cout << "detected vulkan error:" << err << std::endl;\
			abort();\
		}\
	}while(0)\

#ifdef DEBUG
#ifdef WIN32
	static constexpr bool enable_validation_layers = true;
#else
	static constexpr bool enable_validation_layers = false;
#endif
#else
	static constexpr bool enable_validation_layers = false;
#endif

	static const std::vector<const char*> validation_layers = {
		 "VK_LAYER_KHRONOS_validation"
	};

	static const std::vector<const char*> device_extensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
		VK_KHR_MAINTENANCE3_EXTENSION_NAME
	};

	static std::vector<const char*> get_required_extension()
	{
#ifndef Android
		uint32_t glfw_extension_count = 0;
		const char** glfw_extensions;
		glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

		//copy [begin, end] to extensions
		std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);
#else
		std::vector<const char*> extensions;
		extensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif
		if (enable_validation_layers)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_call_back(
		VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}

	static void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info)
	{
		create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		create_info.pfnUserCallback = debug_call_back;
	}

	static VkResult create_debug_utils_messenger_ext(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT*
		p_create_info, const VkAllocationCallbacks* p_allocator, VkDebugUtilsMessengerEXT* p_debug_messenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			return func(instance, p_create_info, p_allocator, p_debug_messenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	static void destroy_debug_utils_messenger_ext(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger,
		const VkAllocationCallbacks* p_allocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

		if (func != nullptr)
		{
			func(instance, debug_messenger, p_allocator);
		}
	}

	static glm::mat4x4 create_projection_matrix_vulkan(uint32_t width, uint32_t height)
	{
		//create ortho projection matrix
		const float left = 0.0f;
		const float right = left + width;
		const float top = 0.0f;
		const float bottom = top + height;
		const float zNear = 0;
		const float zFar = 1;

		//create ortho projection matrix
		//return glm::mat4x4(2.0f / (right - left), 0.0f, 0.0f, -(right + left) / (right - left),
		//	0, 2.0f / (bottom - top), 0.0f, -(bottom + top) / (bottom - top),
		//	0.0f, 0.0f, zFar / (-zNear + zFar), -zNear / (-zNear + zFar),
		//	0.0f, 0.0f, 0.0f, 1.0f
		//);

		return glm::mat4x4(2.0f / (right - left), 0.0f, 0.0f, 0.0f,
			0.0f, 2.0f / (bottom - top), 0.0f, 0.0f,
			0.0f, 0.0f, zFar / (-zNear + zFar), 0.0f,
			-(right + left) / (right - left), -(bottom + top) / (bottom - top), -zNear / (-zNear + zFar), 1.0f);
	}

	//------FSlateVulkanFontAtlasFactory------
	class FSlateVulkanFontAtlasFactory : public ISlateFontAtlasFactory
	{
	public:
		virtual ~FSlateVulkanFontAtlasFactory()
		{

		}

		virtual FIntPoint get_atlas_size(const bool in_is_gray_scale) const override
		{
			return in_is_gray_scale ? FIntPoint(m_gray_scale_texture_size, m_gray_scale_texture_size)
				: FIntPoint(m_color_texture_size, m_color_texture_size);
		}

		virtual std::shared_ptr<FSlateFontAtlas> create_font_atlas(const bool in_is_gray_scale) const override
		{
			const FIntPoint atlas_size = get_atlas_size(in_is_gray_scale);
			return std::make_shared<FSlateFontAtlasVulkan>(atlas_size.x, atlas_size.y, in_is_gray_scale);
		}

		//todo:create non atlased texture

	private:
		/*size of each font texture, width and height*/
		static const uint32_t m_gray_scale_texture_size = 1024;
		static const uint32_t m_color_texture_size = 512;
	};
	//------FSlateVulkanFontAtlasFactory------

	//------font services------
	std::shared_ptr<FSlateFontServices> create_vulkan_font_services()
	{
		const std::shared_ptr<FSlateFontCache> font_cache = std::make_shared<FSlateFontCache>(std::make_shared<FSlateVulkanFontAtlasFactory>());

		return std::make_shared<FSlateFontServices>(font_cache, font_cache);
	}
	//------font services------

	FSlateVulkanRenderer::FSlateVulkanRenderer()
		: Renderer(create_vulkan_font_services())//note:base class holds the font service
	{
		m_b_has_attempted_initialization = false;
		m_view_matrix = glm::mat4x4(1.0f);//identity view matrix
	}

	FSlateVulkanRenderer::~FSlateVulkanRenderer()
	{
		
	}

	FSlateDrawBuffer& FSlateVulkanRenderer::get_draw_buffer()
	{
		m_draw_buffer.clear_buffer();
		//todo:clear buffer
		return m_draw_buffer;
	}

	//bool FSlateVulkanRenderer::Initialize()
	//{
	//	if (!m_b_has_attempted_initialization)
	//	{
	//		//bool b_result = 
	//	}
	//	return true;
	//}

	void FSlateVulkanRenderer::create_view_port(const std::shared_ptr<SWindow> in_window)
	{
		glm::vec2 window_size = in_window->get_size_in_screen();

		private_create_view_port(in_window, window_size);
	}

	bool FSlateVulkanRenderer::request_resize(const std::shared_ptr<SWindow>& in_window, uint32_t new_size_x,
		uint32_t new_size_y)
	{
		//todo:add full screen check
		const bool b_full_screen = false;

		//private_resize_viewport(in_window, new_size_x, new_size_y, b_full_screen);
		private_resize_view_port(in_window, new_size_x, new_size_y, b_full_screen);

		return true;
	}

	void FSlateVulkanRenderer::draw_windows(FSlateDrawBuffer& in_window_draw_buffer)
	{
		//iterate through each element list and set up an rhi window for it if needed
		const std::vector<std::shared_ptr<FSlateWindowElementList>>& window_element_lists = in_window_draw_buffer.get_window_element_lists();

		//m_rendering_policy->clear_vulkan_buffer();

		//std::vector<VkFence> all_windows_fences;
		//for(size_t list_index = 0; list_index < window_element_lists.size(); ++list_index)
		//{
		//	FSlateWindowElementList& element_list = *window_element_lists[list_index];
		//
		//	if (element_list.get_render_window())
		//	{
		//		SWindow* window_to_draw = element_list.get_render_window();
		//		FSlateVulkanViewport& view_port = m_window_to_viewport_map.find(window_to_draw)->second;
		//		all_windows_fences.push_back(view_port.m_fence);
		//	}
		//}
		const std::shared_ptr<FSlateFontCache> font_cache = m_slate_font_services->get_font_cache();

		uint32_t texture_descriptor_offset = 0;

		for(size_t list_index = 0; list_index < window_element_lists.size(); ++list_index)
		{
			FSlateWindowElementList& element_list = *window_element_lists[list_index];

			//if(element_list.get)
			if(element_list.get_render_window())
			{
				SWindow* window_to_draw = element_list.get_render_window();

				m_texture_manager->update_cache();

				//add all elements for this window to the element batcher
				
				//add_elements will add render batch to the element list's batch data array
				m_element_batcher->add_elements(element_list);

				//update the font cache with new text before elements are batched
				font_cache->update_cache();

				FSlateBatchData& batch_data = element_list.get_batch_data();//get the batch data(this is generated by the all elements)

				m_rendering_policy->build_rendering_buffers(m_allocator, batch_data);//generate the vulkan buffer (video memory) to draw

				glm::vec2 window_size = window_to_draw->get_size_in_screen();

				FSlateVulkanViewport& view_port = m_window_to_viewport_map.find(window_to_draw)->second;

				VkDevice device = *(VkDevice*)m_logic_device->get_native_handle();
				VkSwapchainKHR swap_chain = *(VkSwapchainKHR*)view_port.m_vulkan_swap_chain->get_native_handle();

				//note:also wait the all windows element fence, but don't reset last fence

				//wait until the gpu has finished rendering the last frame, timeout of 1 second
				//cpu wait gpu
				VK_CHECK(vkWaitForFences(device, 1, &view_port.m_fence, true, UINT64_MAX));
				VK_CHECK(vkResetFences(device, 1, &view_port.m_fence));

				//request image from the swap chain, one second timeout
				uint32_t swap_chain_image_index;
				//1 seconds is our fps lock
				//image available semaphore
				vkAcquireNextImageKHR(device, swap_chain, UINT64_MAX, view_port.m_present_semaphore, VK_NULL_HANDLE, &swap_chain_image_index);

				VK_CHECK(vkResetCommandBuffer(view_port.m_command_buffer, 0));

				VkCommandBuffer cmd = view_port.m_command_buffer;

				VkCommandBufferBeginInfo cmd_begin_info = {};
				cmd_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				cmd_begin_info.pNext = nullptr;

				cmd_begin_info.pInheritanceInfo = nullptr;
				cmd_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

				VK_CHECK(vkBeginCommandBuffer(cmd, &cmd_begin_info));

				VkClearValue clearValue;
				//float flash = abs(sin(m_frame_number / 120.f));
				clearValue.color = { { 1.0f, 1.0f, 1.0f, 1.0f } };

				VkRenderPassBeginInfo rpInfo = {};
				rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				rpInfo.pNext = nullptr;

				rpInfo.renderPass = m_render_pass;
				rpInfo.renderArea.offset.x = 0;
				rpInfo.renderArea.offset.y = 0;
				std::pair<uint32_t, uint32_t> extent = view_port.m_vulkan_swap_chain->get_swap_chain_extent();
				VkExtent2D vk_extent;
				vk_extent.width = extent.first;
				vk_extent.height = extent.second;//todo:reinterpret cast
				rpInfo.renderArea.extent = vk_extent;
				rpInfo.framebuffer = view_port.m_vulkan_framebuffer[swap_chain_image_index];//frame buffer

				//connect clear values
				rpInfo.clearValueCount = 1;
				rpInfo.pClearValues = &clearValue;

				vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

				//todo:bind pso
				vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, *static_cast<VkPipeline*>(m_pipeline_state_object->get_native_handle()));

				//todo:bind vertex buffer

				//todo:bind view port
				vkCmdSetViewport(cmd, 0, 1, &view_port.m_view_port_info);

				VkRect2D scissor{};
				scissor.offset = { 0, 0 };
				scissor.extent.width = view_port.m_view_port_info.width;
				scissor.extent.height = view_port.m_view_port_info.height;

				vkCmdSetScissor(cmd, 0, 1, &scissor);

				//todo:draw
				m_rendering_policy->draw_elements(device,  cmd, *(VkPipelineLayout*)(m_pipeline_state_object->get_pipeline_layout()),m_sampler,m_view_matrix * view_port.m_projection_matrix, batch_data.get_first_render_batch_index(), batch_data.get_render_batches(), batch_data.get_total_vertex_offset(), batch_data.get_total_index_offset(), texture_descriptor_offset);

				vkCmdEndRenderPass(cmd);

				VK_CHECK(vkEndCommandBuffer(cmd));

				VkSubmitInfo submit = {};
				submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submit.pNext = nullptr;

				VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

				submit.pWaitDstStageMask = &waitStage;

				submit.waitSemaphoreCount = 1;
				submit.pWaitSemaphores = &view_port.m_present_semaphore;//image available semaphore

				submit.signalSemaphoreCount = 1;
				submit.pSignalSemaphores = &view_port.m_render_semaphore;

				submit.commandBufferCount = 1;
				submit.pCommandBuffers = &cmd;

				VkQueue queue = *(VkQueue*)m_logic_device->get_graphics_queue();//all commands submit to one queue

				VK_CHECK(vkQueueSubmit(queue, 1, &submit, view_port.m_fence));

				VkPresentInfoKHR presentInfo = {};
				presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
				presentInfo.pNext = nullptr;

				presentInfo.pSwapchains = &swap_chain;
				presentInfo.swapchainCount = 1;

				presentInfo.pWaitSemaphores = &view_port.m_render_semaphore;
				presentInfo.waitSemaphoreCount = 1;

				presentInfo.pImageIndices = &swap_chain_image_index;

				VK_CHECK(vkQueuePresentKHR(queue, &presentInfo));

				//last_windows_fences.push_back(view_port.m_fence);

				//all elements have been drawn, reset all cached data
				//m_element_batcher->reset
			}
		}

		++m_frame_number;

		//flush the cache if needed

		m_rendering_policy->reset_offset();

		//flush the cache if needed
	}

	void FSlateVulkanRenderer::on_window_destroyed(const std::shared_ptr<SWindow>& in_window)
	{
		VkDevice device = *(VkDevice*)m_logic_device->get_native_handle();
		vkDeviceWaitIdle(device);
		auto it = m_window_to_viewport_map.find(in_window.get());
		if(it != m_window_to_viewport_map.end())
		{
			it->second.m_deletion_queue.flush();
			m_window_to_viewport_map.erase(in_window.get());
		}
	}

	FSlateResourceHandle FSlateVulkanRenderer::get_resource_handle(const FSlateBrush& brush, glm::vec2 local_size, float draw_scale)
	{
		return m_texture_manager->get_resource_handle(brush, local_size, draw_scale);
	}

	bool FSlateVulkanRenderer::initialize()
	{
		if(!m_b_has_attempted_initialization)
		{
			bool b_result = create_device();//create device

			if(b_result)
			{
				VkDevice device = *(VkDevice*)m_logic_device->get_native_handle();
				//create vma
				VmaAllocatorCreateInfo allocator_info = {};
				allocator_info.physicalDevice = m_physical_device;
				allocator_info.device = device;
				allocator_info.instance = m_vulkan_instance;
				vmaCreateAllocator(&allocator_info, &m_allocator);

				m_deletion_queue.push_function([=]()
				{
					vmaDestroyAllocator(m_allocator);
				});

				//todo:implement create texture manager
				create_sync_objects_for_immediate_upload(device);

				m_texture_manager = std::make_shared<FSlateVulkanTextureManager>();

				m_texture_manager->load_used_textures();//load the texture centrally again

				//todo:implement rendering policy
				m_rendering_policy = std::make_shared<FSlateVulkanRenderingPolicy>(m_slate_font_services, m_allocator, m_texture_manager);//note:vma need first initialize
				m_rendering_policy->reset_offset();

				//todo:implement element batcher
				m_element_batcher = std::make_unique<FSlateElementBatcher>(m_rendering_policy);

#ifdef Android
				m_vertex_shader_module = Shader::Create("SlateDefaultVertexShader.spv", &device);
				m_fragment_shader_module = Shader::Create("SlateElementPixelShader.spv", &device);
#else
				DoDoUtf8String vertex_shader_path = FPaths::engine_dir() / "Shader//SlateDefaultVertexShader.spv";
				DoDoUtf8String fragment_shader_path = FPaths::engine_dir() / "Shader//SlateElementPixelShader.spv";
				m_vertex_shader_module = Shader::Create(vertex_shader_path.c_str(), &device);
				m_fragment_shader_module = Shader::Create(fragment_shader_path.c_str(), &device);
#endif
				m_pipeline_state_object = PipelineStateObject::Create(&device);

				std::string target_point = "main";//todo:modify this to other string type

				m_pipeline_state_object->set_vertex_shader(*m_vertex_shader_module, target_point);
				m_pipeline_state_object->set_pixel_shader(*m_fragment_shader_module, target_point);
				m_pipeline_state_object->set_render_pass(&m_render_pass);
				VertexInputDescription input_description = get_vertex_description();
				m_pipeline_state_object->set_input_vertex_layout(&input_description);//todo:get address
				m_pipeline_state_object->set_descriptor_set(1, &m_shader_set_layout);//set descriptor set layout
				m_pipeline_state_object->finalize(&device);//todo:fix me
			}
		}

		m_b_has_attempted_initialization = true;
		return m_b_has_attempted_initialization;
	}

	void FSlateVulkanRenderer::destroy()
	{
		Renderer::destroy();

		VkDevice device = *(VkDevice*)m_logic_device->get_native_handle();

		vkDeviceWaitIdle(device);

		m_pipeline_state_object->Destroy(&device);

		m_rendering_policy->clear_vulkan_buffer(m_allocator);

		m_vertex_shader_module->Destroy(&device);

		m_fragment_shader_module->Destroy(&device);

		for (auto it = m_window_to_viewport_map.begin(); it != m_window_to_viewport_map.end(); ++it) //clear every viewport swap chain objects
		{
			it->second.m_deletion_queue.flush();//every viewport have it's deletion queue
		}

		m_deletion_queue.flush();

		//debug utils messenger need to destroy before vulkan instance
		if (enable_validation_layers)
		{
			destroy_debug_utils_messenger_ext(m_vulkan_instance, m_debug_messenger, nullptr);
		}

		vkDestroyInstance(m_vulkan_instance, nullptr);
	}

	bool FSlateVulkanRenderer::create_device()
	{
		//------get available extensions------
		uint32_t extensions_count = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr);
		std::vector<VkExtensionProperties> available_extensions(extensions_count);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, available_extensions.data());

		std::cout << "available externsions:\n";

		for (const auto& extension : available_extensions)
		{
			std::cout << '\t' << extension.extensionName << '\n';
		}
		//------get available extensions------

		//create application, describe application
		VkApplicationInfo app_info{};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;//describe type
		app_info.pApplicationName = "DoDoUI";
		app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		app_info.pEngineName = "No Engine";
		app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		app_info.apiVersion = VK_API_VERSION_1_0;
		//it has pNext mamber, pointer to extension

		//instance describe application
		VkInstanceCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pApplicationInfo = &app_info;

		//------enable extension------
		static std::vector<const char*> extensions = get_required_extension();//get glfw and debug utils extensions 
		extensions.push_back("VK_KHR_surface");//surface extensions
		extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
		//extensions.push_back("VK_EXT_descriptor_indexing");
		create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		create_info.ppEnabledExtensionNames = extensions.data();
		//------enable extension------

		//------enable validation layers------
		VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
		if (enable_validation_layers)
		{
			create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
			create_info.ppEnabledLayerNames = validation_layers.data();
			//create_info.ppEnabledLyer = validation_layers.data();

			populate_debug_messenger_create_info(debug_create_info);//set debug messenger debug
			create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
		}
		else
		{
			create_info.enabledLayerCount = 0;

			create_info.pNext = nullptr;
		}
		//------enable validation layers------

		VkResult result = vkCreateInstance(&create_info, nullptr, &m_vulkan_instance);

		//------after create instance to create debug messenger------
		setup_debug_message();
		//------after create instance to create debug messenger------

		//------pick a adapter------
		m_physical_device = VK_NULL_HANDLE;
		pick_physical_device();
		//------pick a adapter------

		//------create logic device------
		m_logic_device = Device::create(&m_physical_device, m_deletion_queue);
		//------create logic device------

		//todo:every swap chain have their sync objects
		//------create sync objects------
		//create_sync_objects();
		//------create sync objects------

		//------create render pass-------
		init_default_render_pass();
		//------create render pass-------

		//------create command pool------
		create_command_pool();
		//------create command pool------

		init_descriptors();

		return result == VK_SUCCESS ? true : false;
	}

	void FSlateVulkanRenderer::load_style_resources(const ISlateStyle& style)
	{
		//todo:call texture manager
		m_texture_manager->load_style_resources(style);//create real video memory texture
	}

	void FSlateVulkanRenderer::private_create_view_port(std::shared_ptr<SWindow> in_window, glm::vec2& window_size)
	{
		//get native window handle
		std::shared_ptr<Window> native_window = in_window->get_native_window();

		VkSurfaceKHR surface;
		//create window surface
		create_surface(*native_window, surface);

		//todo:implement create swap chain, create render target, create projection matrix
		FSlateVulkanViewport viewport;
		viewport.m_vulkan_surface = surface;

		viewport.m_deletion_queue.push_function([=]()
		{
			vkDestroySurfaceKHR(m_vulkan_instance, surface, nullptr);
		});

		//get logic device native handle
		VkDevice* device = (VkDevice*)(m_logic_device->get_native_handle());

		viewport.m_vulkan_swap_chain = std::static_pointer_cast<VulkanSwapChain>(SwapChain::Create(&m_physical_device, device, &surface, *native_window, viewport.m_deletion_queue));

		//create framebuffer, frame buffer connect the render pass and image
		viewport.m_vulkan_framebuffer = viewport.m_vulkan_swap_chain->create_frame_buffer(*device, m_render_pass, viewport.m_deletion_queue);

		create_sync_objects(viewport);

		create_command_buffer(viewport);

		viewport.m_view_port_info.maxDepth = 1.0f;
		viewport.m_view_port_info.minDepth = 0.0f;
		viewport.m_view_port_info.width = window_size.x;
		viewport.m_view_port_info.height = window_size.y;
		viewport.m_view_port_info.x = 0.0f;
		viewport.m_view_port_info.y = 0.0f;

		//todo:create projection matrix
		viewport.m_projection_matrix = create_projection_matrix_vulkan(window_size.x, window_size.y);

		m_window_to_viewport_map.insert({in_window.get(), viewport});
	}

	void FSlateVulkanRenderer::private_resize_view_port(const std::shared_ptr<SWindow> in_window, uint32_t width,
		uint32_t height, bool b_full_screen)
	{
		auto it = m_window_to_viewport_map.find(in_window.get());

		FSlateVulkanViewport* viewport = &(it->second);

		std::shared_ptr<Window> native_window = in_window->get_native_window();

		if(viewport && (width != viewport->m_view_port_info.width || height != viewport->m_view_port_info.height || b_full_screen != viewport->m_b_full_screen))
		{
			VkDevice* device = (VkDevice*)m_logic_device->get_native_handle();

			vkDeviceWaitIdle(*device);

			//destroy viewport related objects
			it->second.m_deletion_queue.flush();

			//create new surface
			create_surface(*native_window, it->second.m_vulkan_surface);

			it->second.m_deletion_queue.push_function([=]()
			{
				vkDestroySurfaceKHR(m_vulkan_instance, it->second.m_vulkan_surface, nullptr);
			});

			it->second.m_vulkan_swap_chain = std::static_pointer_cast<VulkanSwapChain>(SwapChain::Create(&m_physical_device, device, &it->second.m_vulkan_surface, *native_window, it->second.m_deletion_queue));

			it->second.m_vulkan_framebuffer = it->second.m_vulkan_swap_chain->create_frame_buffer(*device, m_render_pass, it->second.m_deletion_queue);

			it->second.m_view_port_info.width = width;
			it->second.m_view_port_info.height = height;

			it->second.m_projection_matrix = create_projection_matrix_vulkan(width, height);
		}
	}

	void FSlateVulkanRenderer::pick_physical_device()
	{
		//enumerate physical device
		uint32_t device_count = 0;
		vkEnumeratePhysicalDevices(m_vulkan_instance, &device_count, nullptr);

		if (device_count == 0)
		{
			LOGE("failed to find gpus with vulkan support!");
			//std::cout << "failed to find gpus with vulkan support!" << std::endl;
		}

		std::vector<VkPhysicalDevice> devices(device_count);
		vkEnumeratePhysicalDevices(m_vulkan_instance, &device_count, devices.data());

		//pick up first device
		m_physical_device = devices[0];

		//std::multimap<int32_t, VkPhysicalDevice> candiates;

		//for (const auto& device : devices)
		//{
		//    if (is_device_suitable(device))
		//    {
		//        m_physical_device = device;
		//    }
		//}
		VkPhysicalDeviceProperties gpu_properties;
		vkGetPhysicalDeviceProperties(m_physical_device, &gpu_properties);
		LOGI("Vulkan Physical Device Name: %s\n", gpu_properties.deviceName);
		LOGI("Vulkan Physical Device Info: apiVersion: %x \n\t driverVersion: %x\n", gpu_properties.apiVersion, gpu_properties.driverVersion);
		LOGI("API Version Supported: %d.%d.%d\n",
			VK_VERSION_MAJOR(gpu_properties.apiVersion),
			VK_VERSION_MINOR(gpu_properties.apiVersion),
			VK_VERSION_PATCH(gpu_properties.apiVersion));

		if (m_physical_device == VK_NULL_HANDLE)
		{
			std::cout << "failed to find a suitable gpu!\n" << std::endl;
		}
	}

	void FSlateVulkanRenderer::setup_debug_message()
	{
		if (!enable_validation_layers) return;

		//------create messenger call back------
		VkDebugUtilsMessengerCreateInfoEXT create_info;
		populate_debug_messenger_create_info(create_info);

		if (create_debug_utils_messenger_ext(m_vulkan_instance, &create_info, nullptr, &m_debug_messenger)) {
			std::cout << "create debug messenger object error!" << std::endl;
		}
		else
		{
			std::cout << "create debug messenger object success!" << std::endl;
		}
		//------create messenger call back------
	}

	void FSlateVulkanRenderer::create_surface(Window& window, VkSurfaceKHR& surface)
	{
#ifndef Android
		GLFWwindow* window_handle = (GLFWwindow*)(window.get_window_native_handle());

		VkResult result = glfwCreateWindowSurface(m_vulkan_instance, window_handle, nullptr, &surface);

		if (result != VK_SUCCESS)
		{
			std::cout << "create surface error!" << std::endl;
		}
#else
		ANativeWindow* window_handle = (ANativeWindow*)(window.get_window_native_handle());
		VkAndroidSurfaceCreateInfoKHR create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
		create_info.window = window_handle;

		VkResult result = vkCreateAndroidSurfaceKHR(m_vulkan_instance, &create_info, nullptr, &surface);

		if (result != VK_SUCCESS)
		{
			LOGE("create android surface error!");
		}
#endif

		//m_deletion_queue.push_function([=]()
		//{
		//	vkDestroySurfaceKHR(m_vulkan_instance, surface, nullptr);
		//});

		//this delete is managed by every viewport

	}

	void FSlateVulkanRenderer::create_sync_objects(FSlateVulkanViewport& view_port)
	{
		VkFenceCreateInfo fence_create_info{};
		fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VkDevice device = *(VkDevice*)m_logic_device->get_native_handle();

		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = nullptr;
		semaphoreCreateInfo.flags = 0;

		if(vkCreateFence(device, &fence_create_info, nullptr, &view_port.m_fence) != VK_SUCCESS
		|| vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &view_port.m_present_semaphore) != VK_SUCCESS
		|| vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &view_port.m_render_semaphore) != VK_SUCCESS)
		{
			std::cout << "failed to create sync objects" << std::endl;
		}

		m_deletion_queue.push_function([=]()
		{
			vkDestroyFence(device, view_port.m_fence, nullptr);
			vkDestroySemaphore(device, view_port.m_present_semaphore, nullptr);
			vkDestroySemaphore(device, view_port.m_render_semaphore, nullptr);
		});
	}

	void FSlateVulkanRenderer::create_sync_objects_for_immediate_upload(VkDevice device)
	{
		VkFenceCreateInfo upload_fence_create_info = {};
		upload_fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		//upload_fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		vkCreateFence(device, &upload_fence_create_info, nullptr, &m_upload_context.m_upload_fence);

		m_deletion_queue.push_function([=]() {
			vkDestroyFence(device, m_upload_context.m_upload_fence, nullptr);
		});
	}

	void FSlateVulkanRenderer::init_frame_buffers()
	{
		//create the framebuffers for the swapchain images. This will connect the render-pass to the images for rendering


	}

	void FSlateVulkanRenderer::init_default_render_pass()
	{
		//we define an attachment description for our main color image
		//the attachment is loaded as "clear" when renderpass start
		//the attachment is stored when renderpass ends
		//the attachment layout starts as "undefined", and transitions to "Present" so its possible to display it
		//we dont care about stencil, and dont use multisampling

		VkAttachmentDescription color_attachment = {};
		color_attachment.format = VK_FORMAT_B8G8R8A8_UNORM;//todo:need to fix this, this is related to swap chain image or render target image
		color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference color_attachment_ref = {};
		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		//we are going to create 1 subpass, which is the minimum you can do
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment_ref;

		//1 dependency, which is from "outside" into the subpass. And we can read or write color
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo render_pass_info = {};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		render_pass_info.attachmentCount = 1;
		render_pass_info.pAttachments = &color_attachment;
		render_pass_info.subpassCount = 1;
		render_pass_info.pSubpasses = &subpass;
		render_pass_info.dependencyCount = 1;
		render_pass_info.pDependencies = &dependency;

		VkDevice device = *(VkDevice*)m_logic_device->get_native_handle();

		VK_CHECK(vkCreateRenderPass(device, &render_pass_info, nullptr, &m_render_pass));

		m_deletion_queue.push_function([=]()
		{
			vkDestroyRenderPass(device, m_render_pass, nullptr);
		});
	}

	void FSlateVulkanRenderer::init_descriptors()
	{
		//binding
		VkDescriptorSetLayoutBinding shader_param = descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, 0);

		VkDescriptorSetLayoutBinding texture = descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1);

		VkDescriptorBindingFlags flags[2] = { VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT, VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT };
		VkDescriptorSetLayoutBindingFlagsCreateInfo bind_flags = {};
		bind_flags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
		bind_flags.bindingCount = 2;
		bind_flags.pBindingFlags = flags;
		bind_flags.pNext = nullptr;

		VkDescriptorSetLayoutCreateInfo setinfo = {};

		setinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		setinfo.pNext = &bind_flags;

		VkDescriptorSetLayoutBinding bindings[] = { shader_param, texture };

		//we are going to have 2 binding
		setinfo.bindingCount = 2;
		//no flags
		setinfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
		//point to the camera buffer binding
		setinfo.pBindings = bindings;

		VkDevice device = *(VkDevice*)m_logic_device->get_native_handle();

		vkCreateDescriptorSetLayout(device, &setinfo, nullptr, &m_shader_set_layout);

		std::vector<VkDescriptorPoolSize> sizes =
		{
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2005 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10},
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10},
			//add combined-image-sampler descriptor types to the pool
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 20005 }
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = (uint32_t)sizes.size();
		pool_info.pPoolSizes = sizes.data();

		vkCreateDescriptorPool(device, &pool_info, nullptr, &m_descriptor_pool);

		m_deletion_queue.push_function([=] {
			vkDestroyDescriptorSetLayout(device, m_shader_set_layout, nullptr);
			vkDestroyDescriptorPool(device, m_descriptor_pool, nullptr);
		});

		//create descriptor set

		//------------------create specific sampler and texture descriptor------------------

		//create a sampler for the texture
		VkSamplerCreateInfo sampler_info = sampler_create_info(VK_FILTER_NEAREST);//todo:fix me

		//VkSampler sampler;

		vkCreateSampler(device, &sampler_info, nullptr, &m_sampler);

		m_deletion_queue.push_function([=] {
			vkDestroySampler(device, m_sampler, nullptr);
		});

		//allocate the descriptor set for texture to use on the material
		//VkDescriptorSetAllocateInfo alloc_info = {};
		//alloc_info.pNext = nullptr;
		//alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		//alloc_info.descriptorPool = m_descriptor_pool;
		//alloc_info.descriptorSetCount = 1;
		//alloc_info.pSetLayouts = &m_shader_set_layout;
		//
		//vkAllocateDescriptorSets(device, &alloc_info, &m_descriptor_set);

		//write to the descriptor set so that it points to our empire_diffuse texture
		//VkDescriptorImageInfo imageBufferInfo;
		//imageBufferInfo.sampler = sampler;
		//imageBufferInfo.imageView = ;//get the image view
		//imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		//
		//VkWriteDescriptorSet texture1 = write_descriptor_image(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, m_descriptor_set, &imageBufferInfo, 0);
		//
		//vkUpdateDescriptorSets(device, 1, &texture1, 0, nullptr);
	}

	VertexInputDescription FSlateVulkanRenderer::get_vertex_description()
	{
		VertexInputDescription description = {};

		VkVertexInputBindingDescription mainBinding = {};
		mainBinding.binding = 0;
		mainBinding.stride = sizeof(FSlateVertex);
		mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		description.bindings.push_back(mainBinding);

		VkVertexInputAttributeDescription texcoordAttribute = {};
		texcoordAttribute.binding = 0;
		texcoordAttribute.location = 0;
		texcoordAttribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		texcoordAttribute.offset = offsetof(FSlateVertex, tex_coords);

		VkVertexInputAttributeDescription positionAttribute = {};
		positionAttribute.binding = 0;
		positionAttribute.location = 1;
		positionAttribute.format = VK_FORMAT_R32G32_SFLOAT;
		positionAttribute.offset = offsetof(FSlateVertex, m_position);

		VkVertexInputAttributeDescription colorAttribute = {};
		colorAttribute.binding = 0;
		colorAttribute.location = 2;
		colorAttribute.format = VK_FORMAT_R8G8B8A8_UNORM;
		colorAttribute.offset = offsetof(FSlateVertex, m_color);//todo:implement as UNORM

		VkVertexInputAttributeDescription color2Attribute = {};
		color2Attribute.binding = 0;
		color2Attribute.location = 3;
		color2Attribute.format = VK_FORMAT_R8G8B8A8_UNORM;
		color2Attribute.offset = offsetof(FSlateVertex, m_secondary_color);

		description.attributes.push_back(texcoordAttribute);
		description.attributes.push_back(positionAttribute);
		description.attributes.push_back(colorAttribute);
		description.attributes.push_back(color2Attribute);
		return description;
	}

	void FSlateVulkanRenderer::immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function)
	{		
		VkCommandBuffer cmd = m_upload_context.m_command_buffer;

		//begin the command buffer recording. We will use this command buffer exactly once before resetting, so we tell vulkan that
		VkCommandBufferBeginInfo cmdBeginInfo = command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

		//execute the function
		function(cmd);

		VK_CHECK(vkEndCommandBuffer(cmd));

		VkSubmitInfo submit = submit_info(&cmd);

		VkQueue queue = *(VkQueue*)m_logic_device->get_graphics_queue();//get queue
		VkDevice device = *(VkDevice*)m_logic_device->get_native_handle();

		//submit command buffer to the queue and execute it.
		// _uploadFence will now block until the graphic commands finish execution
		VK_CHECK(vkQueueSubmit(queue, 1, &submit, m_upload_context.m_upload_fence));

		vkWaitForFences(device, 1, &m_upload_context.m_upload_fence, true, 9999999999);
		vkResetFences(device, 1, &m_upload_context.m_upload_fence);

		// reset the command buffers inside the command pool
		vkResetCommandPool(device, m_upload_context.m_commad_pool, 0);
	}

	AllocatedBuffer FSlateVulkanRenderer::create_buffer(size_t allocated_size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
	{
		//allocate vertex buffer
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.pNext = nullptr;

		bufferInfo.size = allocated_size;
		bufferInfo.usage = usage;

		VmaAllocationCreateInfo vmaallocInfo = {};
		vmaallocInfo.usage = memoryUsage;

		AllocatedBuffer newBuffer;

		//allocate the buffer
		VK_CHECK(vmaCreateBuffer(m_allocator, &bufferInfo, &vmaallocInfo,
			&newBuffer.m_buffer,
			&newBuffer.m_allocation,
			nullptr));

		return newBuffer;
	}

	void FSlateVulkanRenderer::create_command_pool()
	{
		VkDevice device = *(VkDevice*)m_logic_device->get_native_handle();

		std::optional<uint32_t> queue_family_index = VulkanUtils::find_queue_families(m_physical_device);//interms of physical device to find queue family index

		VkCommandPoolCreateInfo pool_info{};
		pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		pool_info.queueFamilyIndex = queue_family_index.value();

		if (vkCreateCommandPool(device, &pool_info, nullptr, &m_command_pool) != VK_SUCCESS)
		{
			std::cout << "create command pool error!" << std::endl;
		}

		m_deletion_queue.push_function([=]()
		{
			vkDestroyCommandPool(device, m_command_pool, nullptr);
		});

		//------upload context------
		VkCommandPoolCreateInfo upload_command_pool_info = command_pool_create_info(queue_family_index.value());

		vkCreateCommandPool(device, &upload_command_pool_info, nullptr, &m_upload_context.m_commad_pool);

		m_deletion_queue.push_function([=]() {
			vkDestroyCommandPool(device, m_upload_context.m_commad_pool, nullptr);
		});

		//allocate the default command buffer that we will use for the instant commands
		VkCommandBufferAllocateInfo cmd_alloc_info = command_buffer_allocate_info(m_upload_context.m_commad_pool, 1);

		VkCommandBuffer cmd;
		vkAllocateCommandBuffers(device, &cmd_alloc_info, &m_upload_context.m_command_buffer);
		//------upload context------
	}

	void FSlateVulkanRenderer::create_command_buffer(FSlateVulkanViewport& view_port)
	{
		VkDevice device = *(VkDevice*)m_logic_device->get_native_handle();
		VkCommandBufferAllocateInfo alloc_info{};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.commandPool = m_command_pool;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(device, &alloc_info, &view_port.m_command_buffer) != VK_SUCCESS)
		{
			std::cout << "failed to allocate command buffers" << std::endl;
		}
	}
}
