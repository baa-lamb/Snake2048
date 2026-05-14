#pragma once

#include <vulkan/vulkan.h>
#include <optional>

// Holds the indices of the queue families we need
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;

    bool isComplete() {
        return graphicsFamily.has_value();
    }
};

VkPhysicalDevice pickPhysicalDevice(VkInstance& instance);

// Search a physical device for the queue families we need
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

