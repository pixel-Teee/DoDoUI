#include <PreCompileHeader.h>
#include "Utils.h"
#include <vulkan/vulkan.h>

namespace DoDo {
    namespace VulkanUtils {
        QueueFamilyIndices find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface)
        {
            {
                QueueFamilyIndices indices;

                //logic to find graphics queue family
                uint32_t queue_family_count = 0;

                vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

                std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);

                vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

                int32_t i = 0;
                for (const auto& queue_families : queue_families)
                {
                    if (queue_families.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    {
                        indices.graphics_family = i;
                    }

                    VkBool32 present_support = false;
                    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);

                    if (present_support)
                    {
                        indices.present_family = i;
                    }

                    if (indices.is_complete())
                    {
                        break;
                    }

                    ++i;
                }

                return indices;
            }
        }
        SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device, VkSurfaceKHR surface)
        {
            SwapChainSupportDetails details;

            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

            uint32_t format_count;
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);

            if (format_count != 0)
            {
                details.formats.resize(format_count);
                vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats.data());
            }

            uint32_t present_mode_count;
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);

            if (present_mode_count != 0)
            {
                details.present_modes.resize(present_mode_count);
                vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.present_modes.data());
            }

            return details;
        }
    }

}

        