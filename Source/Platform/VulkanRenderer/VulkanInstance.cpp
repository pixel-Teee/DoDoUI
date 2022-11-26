#include "VulkanInstance.h"

//------vulkan for glfw------
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
//------vulkan for glfw------

namespace DoDo {
#ifdef DEBUG
    static constexpr bool enable_validation_layers = true;
#else
    static constexpr bool enable_validation_layers = false;
#endif

   static const std::vector<const char*> validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };

   static std::vector<const char*> get_required_extension()
    {
        uint32_t glfw_extension_count = 0;
        const char** glfw_extensions;
        glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

        //copy [begin, end] to extensions
        std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

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

	VulkanInstance::VulkanInstance()
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

        //from glfw to get extensions
        uint32_t glfw_extension_count = 0;
        const char** glfw_extensions;

        //glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

        //------extension------
        static std::vector<const char*> extensions = get_required_extension();
        create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        create_info.ppEnabledExtensionNames = extensions.data();
        //------extension------

        //------enable validation layers------
        VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
        if (enable_validation_layers)
        {
            create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
            create_info.ppEnabledLayerNames = validation_layers.data();
            //create_info.ppEnabledLyer = validation_layers.data();

            populate_debug_messenger_create_info(debug_create_info);
            create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
        }
        else
        {
            create_info.enabledLayerCount = 0;

            create_info.pNext = nullptr;
        }
        //------enable validation layers------

        VkResult result = vkCreateInstance(&create_info, nullptr, &m_vulkan_instance);

        if (result != VK_SUCCESS)
        {
            std::cout << "create vulkan instance error!" << std::endl;
        }
        else
        {
            std::cout << "create vulkan instance success!" << std::endl;
        }

        //-----after create instance to create debug messenger------
        setup_debug_message();
        //-----after create instance to create debug messenger------
	}
     
	VulkanInstance::~VulkanInstance()
	{
        if (enable_validation_layers)
        {
            destroy_debug_utils_messenger_ext(m_vulkan_instance, m_debug_messenger, nullptr);
        }

		vkDestroyInstance(m_vulkan_instance, nullptr);
	}

    void VulkanInstance::setup_debug_message()
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
}
