#pragma once

#include <optional>
#include <vulkan/vulkan.h>

#include <include/vk_mem_alloc.h>

namespace DoDo {
    namespace VulkanUtils {
        struct QueueFamilyIndices {
            std::optional<uint32_t> graphics_family;
            std::optional<uint32_t> present_family;

            bool is_complete() {
                return graphics_family.has_value() && present_family.has_value();
            }
        };

        std::optional<uint32_t> find_queue_families(VkPhysicalDevice device);

        QueueFamilyIndices find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface);

        struct SwapChainSupportDetails {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> present_modes;
        };

        SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device, VkSurfaceKHR surface);
    }

    struct AllocatedBuffer
    {
        VkBuffer m_buffer;
        VmaAllocation m_allocation;//record the extra state information about VkBuffer

        AllocatedBuffer() { m_buffer = {}, m_allocation = {}; }//clear
    };
}

