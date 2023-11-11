#include <PreCompileHeader.h>
#include "VulkanInstance.h"

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

#include "Core/Window.h"
#include "Core/GenericPlatform/GenericPlatformMisc.h"

#include "Platform/VulkanRenderer/VulkanLogicDevice.h"
#include "Platform/VulkanRenderer/VulkanSwapChain.h"
#include "Platform/VulkanRenderer/VulkanShader.h"
#include "Platform/VulkanRenderer/VulkanPipelineStateObject.h"
#include "Utils.h"

namespace DoDo {
#ifdef DEBUG
    static constexpr bool enable_validation_layers = false;
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

	VulkanInstance::VulkanInstance(Window& window)
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
        //uint32_t glfw_extension_count = 0;
        //const char** glfw_extensions;

        //glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

        //------extension------
        static std::vector<const char*> extensions = get_required_extension();
        extensions.push_back("VK_KHR_surface");//surface extensions
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
            //std::cout << "create vulkan instance error!" << std::endl;
            LOGE("create vulkan instance error!");
        }
        else
        {
            //std::cout << "create vulkan instance success!" << std::endl;
            LOGI("create vulkan instance success!");
        }

        //-----after create instance to create debug messenger------
        setup_debug_message();
        //-----after create instance to create debug messenger------      

        //-----create surface------
        create_surface(window);
        //-----create surface------

        //-----pick a adapter------
        m_physical_device = VK_NULL_HANDLE;
        pick_physical_device();
        //-----pick a adapter------

        //-----create logic device------
        m_p_logic_device = Device::CreateDevice(&m_physical_device, &m_surface);
        //-----create logic device------

        //------test------
        //std::filesystem::path _path = std::filesystem::current_path();
        //std::string _path2 = _path.string();
        //std::string _path3 = "/storage/Shader/vert.spv";
        //bool is_exist = std::filesystem::exists(_path3);
#ifdef Android
        m_vertex_shader_module = Shader::Create("vert.spv", m_p_logic_device->get_native_handle());
        m_fragment_shader_module = Shader::Create("frag.spv", m_p_logic_device->get_native_handle());
#else
        DoDoUtf8String EnginePath = FGenericPlatformMisc::engine_dir();
        DoDoUtf8String VertPath = EnginePath / "Shader/vert.spv";
        DoDoUtf8String FragPath = EnginePath / "Shader/frag.spv";

        m_vertex_shader_module = Shader::Create(VertPath.c_str(), m_p_logic_device->get_native_handle());
        m_fragment_shader_module = Shader::Create(FragPath.c_str(), m_p_logic_device->get_native_handle());
#endif

        m_pipeline_state_object = PipelineStateObject::Create(m_p_logic_device->get_native_handle());

        std::string target_point = "main";

        m_pipeline_state_object->set_vertex_shader(*m_vertex_shader_module, target_point);
        m_pipeline_state_object->set_pixel_shader(*m_fragment_shader_module, target_point);
        m_pipeline_state_object->finalize(m_p_logic_device->get_native_handle());
        //------test------

        //-----create swap chain------
        m_p_swap_chain = SwapChain::Create(&m_physical_device, m_p_logic_device->get_native_handle(), &m_surface, m_pipeline_state_object->get_render_pass_native_handle(), window);
        //-----create swap chain------

        //-----test------
        create_command_pool();
        create_command_buffer();
        create_sync_objects();
        //-----test------
	}
     
	VulkanInstance::~VulkanInstance()
	{
        
	}

    void VulkanInstance::Destroy()
    {
        if (enable_validation_layers)
        {
            destroy_debug_utils_messenger_ext(m_vulkan_instance, m_debug_messenger, nullptr);
        }

        VkDevice device = *(VkDevice*)m_p_logic_device->get_native_handle();

        vkDestroySemaphore(device, m_image_available_semaphore, nullptr);

        vkDestroySemaphore(device, m_render_finished_semaphore, nullptr);

        vkDestroyFence(device, m_flight_fence, nullptr);

        vkDestroyCommandPool(device, m_command_pool, nullptr);

        m_pipeline_state_object->Destroy(m_p_logic_device->get_native_handle());

        m_p_swap_chain->Destroy(m_p_logic_device->get_native_handle());

        m_vertex_shader_module->Destroy(m_p_logic_device->get_native_handle());

        m_fragment_shader_module->Destroy(m_p_logic_device->get_native_handle());

        //device need to destroy at there
        m_p_logic_device->destroy();

        vkDestroySurfaceKHR(m_vulkan_instance, m_surface, nullptr);

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

    bool VulkanInstance::check_device_extension_support(VkPhysicalDevice device)
    {
        //check device support swap khr?
        //check required khr is in the available khr?
        uint32_t extension_count;

        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

        std::vector<VkExtensionProperties> available_extensions(extension_count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

        std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());

        for (const auto& extension : available_extensions)
        {
            required_extensions.erase(extension.extensionName);
        }

        return required_extensions.empty();
    }

    void VulkanInstance::create_command_pool()
    {
        VkDevice device = *(VkDevice*)m_p_logic_device->get_native_handle();
        VulkanUtils::QueueFamilyIndices queue_family_indices = VulkanUtils::find_queue_families(m_physical_device, m_surface);

        VkCommandPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();

        if (vkCreateCommandPool(device, &pool_info, nullptr, &m_command_pool) != VK_SUCCESS)
        {
            std::cout << "create command pool error!" << std::endl;
        }
    }

    void VulkanInstance::create_command_buffer()
    {
        VkDevice device = *(VkDevice*)m_p_logic_device->get_native_handle();
        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = m_command_pool;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(device, &alloc_info, &m_command_buffer) != VK_SUCCESS)
        {
            std::cout << "failed to allocate command buffers" << std::endl;
        }
    }

    void VulkanInstance::record_command_buffer(uint32_t image_index)
    {
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = 0;//optional
        begin_info.pInheritanceInfo = nullptr;//optional

        if (vkBeginCommandBuffer(m_command_buffer, &begin_info) != VK_SUCCESS)
        {
            std::cout << "failed to begin recording command buffer" << std::endl;
        }

        VkRenderPass render_pass = *(VkRenderPass*)m_pipeline_state_object->get_render_pass_native_handle();

        VkFramebuffer frame_buffer = *(VkFramebuffer*)m_p_swap_chain->get_framebuffer(image_index);

        VkRenderPassBeginInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = render_pass;
        render_pass_info.framebuffer = frame_buffer;

        render_pass_info.renderArea.offset = { 0, 0 };

        VkExtent2D extent;
        std::pair<uint32_t, uint32_t> width_and_height = m_p_swap_chain->get_swap_chain_extent();
        extent.width = width_and_height.first;
        extent.height = width_and_height.second;

        render_pass_info.renderArea.extent = extent;

        VkClearValue clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
        render_pass_info.clearValueCount = 1;
        render_pass_info.pClearValues = &clear_color;
        
        vkCmdBeginRenderPass(m_command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
        //render_pass_info.renderArea.extent = m_p_swap_chain->

        VkViewport view_port;
        view_port.width = 1280;
        view_port.height = 720;
        view_port.x = 0;
        view_port.y = 0;
        view_port.minDepth = 0.0f;
        view_port.maxDepth = 1.0f;

        vkCmdSetViewport(m_command_buffer, 0, 1, &view_port);

        VkRect2D scissor;
        //VkExtent2D extent;
        extent.width = 1280;
        extent.height = 720;
        scissor.extent = extent;
        scissor.offset = { 0, 0 };

        vkCmdSetScissor(m_command_buffer, 0, 1, &scissor);

        VkPipeline pipeline = *(VkPipeline*)m_pipeline_state_object->get_native_handle();

        vkCmdBindPipeline(m_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        vkCmdDraw(m_command_buffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(m_command_buffer);

        if (vkEndCommandBuffer(m_command_buffer) != VK_SUCCESS)
        {
            std::cout << "failed to record command buffer!" << std::endl;
        }
    }
    void VulkanInstance::create_sync_objects()
    {
        VkSemaphoreCreateInfo semaphore_create_info{};
        semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_create_info{};
        fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VkDevice device = *(VkDevice*)m_p_logic_device->get_native_handle();

        if ((vkCreateSemaphore(device, &semaphore_create_info, nullptr, &m_image_available_semaphore) != VK_SUCCESS) ||
            (vkCreateSemaphore(device, &semaphore_create_info, nullptr, &m_render_finished_semaphore) != VK_SUCCESS) ||
            (vkCreateFence(device, &fence_create_info, nullptr, &m_flight_fence) != VK_SUCCESS))
        {
            std::cout << "failed to create semaphores" << std::endl;
        }
    }


    void VulkanInstance::draw_frame()
    {
        VkDevice device = *(VkDevice*)m_p_logic_device->get_native_handle();

        vkWaitForFences(device, 1, &m_flight_fence, VK_TRUE, UINT64_MAX);

        vkResetFences(device, 1, &m_flight_fence);

        uint32_t image_index;

        VkSwapchainKHR swap_chain_khr = *(VkSwapchainKHR*)m_p_swap_chain->get_native_handle();

        vkAcquireNextImageKHR(device, swap_chain_khr, UINT64_MAX, m_image_available_semaphore, VK_NULL_HANDLE, &image_index);

        //make sure it to be recorded
        vkResetCommandBuffer(m_command_buffer, 0);

        record_command_buffer(image_index);

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        
        VkSemaphore wait_semaphores[] = { m_image_available_semaphore };
        VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores;
        submit_info.pWaitDstStageMask = wait_stages;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &m_command_buffer;

        VkSemaphore signal_semaphores[] = { m_render_finished_semaphore };
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;

        VkQueue graphics_queue = *(VkQueue*)m_p_logic_device->get_graphics_queue();

        VkQueue present_queue = *(VkQueue*)m_p_logic_device->get_present_queue();

        if (vkQueueSubmit(graphics_queue, 1, &submit_info, m_flight_fence) != VK_SUCCESS)
        {
            std::cout << "failed to submit draw command buffer!" << std::endl;
        }

        VkPresentInfoKHR present_info{};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = signal_semaphores;

        VkSwapchainKHR swap_chains[] = { swap_chain_khr };
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swap_chains;
        present_info.pImageIndices = &image_index;
        present_info.pResults = nullptr;

        vkQueuePresentKHR(present_queue, &present_info);
    }

    void VulkanInstance::create_surface(Window& window)
    {
#ifndef Android
        GLFWwindow* window_handle = (GLFWwindow*)(window.get_window_native_handle());
        VkResult result = glfwCreateWindowSurface(m_vulkan_instance, window_handle, nullptr, &m_surface);

        if(result != VK_SUCCESS)
        {
            std::cout << "create surface error!" << std::endl;
        }
#else
        ANativeWindow* window_handle = (ANativeWindow*)(window.get_window_native_handle());
        VkAndroidSurfaceCreateInfoKHR create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
        create_info.window = window_handle;

        VkResult result = vkCreateAndroidSurfaceKHR(m_vulkan_instance, &create_info, nullptr, &m_surface);

        if(result != VK_SUCCESS)
        {
            LOGE("create android surface error!");
        }
#endif
    }

    bool VulkanInstance::is_device_suitable(VkPhysicalDevice device)
    {
        VulkanUtils::QueueFamilyIndices indices = VulkanUtils::find_queue_families(device, m_surface);

        bool extension_supported = check_device_extension_support(device);

        return indices.is_complete() && extension_supported;
    }

    void VulkanInstance::wait_for_idle()
    {
        VkDevice device = *(VkDevice*)m_p_logic_device->get_native_handle();
        vkDeviceWaitIdle(device);
    }

    void VulkanInstance::pick_physical_device()
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
        LOGI("Vulkan Physical Device Name: %s", gpu_properties.deviceName);
        LOGI("Vulkan Physical Device Info: apiVersion: %x \n\t driverVersion: %x", gpu_properties.apiVersion, gpu_properties.driverVersion);
        LOGI("API Version Supported: %d.%d.%d",
             VK_VERSION_MAJOR(gpu_properties.apiVersion),
             VK_VERSION_MINOR(gpu_properties.apiVersion),
             VK_VERSION_PATCH(gpu_properties.apiVersion));

        if (m_physical_device == VK_NULL_HANDLE)
        {
            std::cout << "failed to find a suitable gpu!" << std::endl;
        }
    }

    
}
