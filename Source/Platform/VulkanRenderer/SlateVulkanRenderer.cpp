#include <PreCompileHeader.h>

#include "SlateVulkanRenderer.h"

#include <include/vk_mem_alloc.h>//todo:need to fix this prefix

#include "SlateCore/Widgets/SWindow.h"

#include "VulkanLogicDevice.h"

#include "Utils.h"
#include "Core/Window.h"//platform window
#include "Renderer/SwapChain.h"//swap chain
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
}
