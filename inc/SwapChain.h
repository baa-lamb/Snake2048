// This file contains Swap chain setup and implementation
// wtf, why it is so complicated..

#pragma once

#include <vulkan/vulkan.h>
#include <vector>

struct GLFWwindow;

namespace sch
{
    
const std::vector<const char*> DeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// Holds all the details we need to create a swap chain
struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;        // Min/max images, size limits
    std::vector<VkSurfaceFormatKHR> formats;       // Pixel format + color space options
    std::vector<VkPresentModeKHR> presentModes;    // FIFO, Mailbox, etc.
};

// Check if a physical device supports the swap chain extension
bool checkDeviceExtensionSupport(VkPhysicalDevice device);

// Method will populate `SwapChainSupportDetails` struct.
SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

// Pick the best surface format (ideal: BGRA 8-bit sRGB)
VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

// Pick the best present mode (ideal: Mailbox)
VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

// Pick the swap extent (image resolution in pixels)
VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

// Create the swap chain and retrieve its images
void createSwapChain(
    VkDevice device,                       // Logical device
    VkPhysicalDevice physicalDevice,       // Physical GPU (for queries)
    VkSurfaceKHR surface,                  // Window surface
    GLFWwindow* window,                    // GLFW window (for pixel size)
    VkSwapchainKHR& swapChain,             // Output: swap chain handle
    std::vector<VkImage>& swapChainImages, // Output: images to render to
    VkFormat& swapChainImageFormat,        // Output: chosen pixel format
    VkExtent2D& swapChainExtent            // Output: chosen resolution
);

// Clean up the swap chain
void cleanupSwapChain(VkDevice device, VkSwapchainKHR swapChain);


} // namespace sch