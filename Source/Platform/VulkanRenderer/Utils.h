#pragma once

#include <optional>
#include <vulkan/vulkan.h>

namespace DoDo {
    namespace VulkanUtils {
        struct QueueFamilyIndices {
            std::optional<uint32_t> graphics_family;
            std::optional<uint32_t> present_family;

            bool is_complete() {
                return graphics_family.has_value() && present_family.has_value();
            }
        };

        QueueFamilyIndices find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface);
    }
    
}

