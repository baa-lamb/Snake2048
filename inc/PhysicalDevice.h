#pragma once

#include <vulkan/vulkan.h>
#include <optional>

// Holds the indices of the queue families we need
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

VkPhysicalDevice pickPhysicalDevice(VkInstance& instance, const VkSurfaceKHR& surface);

// Search a physical device for the queue families we need
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, const VkSurfaceKHR& surface);

