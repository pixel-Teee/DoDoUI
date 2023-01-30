#include <PreCompileHeader.h>

#include "SlateVulkanRenderer.h"

#include <include/vk_mem_alloc.h>//todo:need to fix this prefix

#include "SlateCore/Widgets/SWindow.h"

#include "VulkanLogicDevice.h"

#include "Utils.h"
#include "VulkanSwapChain.h"
#include "Core/Window.h"//platform window
#include "Renderer/SwapChain.h"//swap chain
#include "SlateCore/Rendering/DrawElements.h"
//#include "Renderer/Device.h"

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
	static constexpr bool enable_validation_layers = true;
#else
	static constexpr bool enable_validation_layers = false;
#endif

	static const std::vector<const char*> validation_layers = {
		 "VK_LAYER_KHRONOS_validation"
	};

	static const std::vector<const char*> device_extensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
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
		glm::vec2 window_size = in_window->get_desired_size();

		private_create_view_port(in_window, window_size);
	}

	void FSlateVulkanRenderer::draw_windows(FSlateDrawBuffer& in_window_draw_buffer)
	{
		//iterate through each element list and set up an rhi window for it if needed
		const std::vector<std::shared_ptr<FSlateWindowElementList>>& window_element_lists = in_window_draw_buffer.get_window_element_lists();

		for(size_t list_index = 0; list_index < window_element_lists.size(); ++list_index)
		{
			FSlateWindowElementList& element_list = *window_element_lists[list_index];

			//if(element_list.get)
			if(element_list.get_render_window())
			{
				SWindow* window_to_draw = element_list.get_render_window();

				//add all elements for this window to the element batcher
				//todo:implement element batcher

				glm::vec2 window_size = window_to_draw->get_size_in_screen();

				FSlateVulkanViewport& view_port = m_window_to_viewport_map.find(window_to_draw)->second;

				VkDevice device = *(VkDevice*)m_logic_device->get_native_handle();
				VkSwapchainKHR swap_chain = *(VkSwapchainKHR*)view_port.m_vulkan_swap_chain->get_native_handle();

				//wait until the gpu has finished rendering the last frame, timeout of 1 second
				//cpu wait gpu
				VK_CHECK(vkWaitForFences(device, 1, &view_port.m_fence, true, 1000000000));
				VK_CHECK(vkResetFences(device, 1, &view_port.m_fence));

				//request image from the swap chain, one second timeout
				uint32_t swap_chain_image_index;
				//1 seconds is our fps lock
				VK_CHECK(vkAcquireNextImageKHR(device, swap_chain, 1000000000, view_port.m_present_semaphore, nullptr, &swap_chain_image_index));

				VK_CHECK(vkResetCommandBuffer(view_port.m_command_buffer, 0));

				VkCommandBuffer cmd = view_port.m_command_buffer;

				VkCommandBufferBeginInfo cmd_begin_info = {};
				cmd_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				cmd_begin_info.pNext = nullptr;

				cmd_begin_info.pInheritanceInfo = nullptr;
				cmd_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

				VK_CHECK(vkBeginCommandBuffer(cmd, &cmd_begin_info));

				VkClearValue clearValue;
				float flash = abs(sin(m_frame_number / 120.f));
				clearValue.color = { { 0.0f, 0.0f, flash, 1.0f } };

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
			}
		}

		++m_frame_number;

		//flush the cache if needed
	}

	bool FSlateVulkanRenderer::initialize()
	{
		if(!m_b_has_attempted_initialization)
		{
			bool b_result = create_device();//create device

			if(b_result)
			{
				//todo:implement create texture manager

				//todo:implement rendering policy

				//todo:implement element batcher
			}
		}

		m_b_has_attempted_initialization = true;
		return m_b_has_attempted_initialization;
	}

	void FSlateVulkanRenderer::destroy()
	{
		Renderer::destroy();

		//vkDeviceWaitIdle();

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

		return result == VK_SUCCESS ? true : false;
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

		//get logic device native handle
		VkDevice* device = (VkDevice*)(m_logic_device->get_native_handle());

		viewport.m_vulkan_swap_chain = std::reinterpret_pointer_cast<VulkanSwapChain>(SwapChain::Create(&m_physical_device, device, &surface, *native_window, m_deletion_queue));

		//create framebuffer, frame buffer connect the render pass and image
		viewport.m_vulkan_framebuffer = viewport.m_vulkan_swap_chain->create_frame_buffer(*device, m_render_pass, m_deletion_queue);

		create_sync_objects(viewport);

		create_command_buffer(viewport);

		m_window_to_viewport_map.insert({in_window.get(), viewport});
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

		m_deletion_queue.push_function([=]()
		{
			vkDestroySurfaceKHR(m_vulkan_instance, surface, nullptr);
		});

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
