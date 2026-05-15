#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "SwapChain.h"
#include "PhysicalDevice.h"

#include <iostream>
#include <cstdint> 
#include <limits> 
#include <algorithm>
#include <set>
#include <string>

namespace sch
{

// --------------------------- helpers ------------------------------

bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }
    return requiredExtensions.empty();
}

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	SwapChainSupportDetails details;

	// 1. basic: min/max img count, ... 
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
	
	// 2. surface formats
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	// 3. presentation modes: FIFO, Mailbox, ... 
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	// if ideal format was not found:
	return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	// Only the VK_PRESENT_MODE_FIFO_KHR mode is guaranteed to be available.
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	} else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>( width ),
			static_cast<uint32_t>( height )
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

// ---------------------------------- create swap chain --------------------------------------
void createSwapChain(
	VkDevice device,                       
	VkPhysicalDevice physicalDevice,       
	VkSurfaceKHR surface,                  
	GLFWwindow* window,                    
	VkSwapchainKHR& swapChain,             
	std::vector<VkImage>& swapChainImages, 
	VkFormat& swapChainImageFormat,        
	VkExtent2D& swapChainExtent            
)
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);

	// 1. choose best settings
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window);

	// 2. how many images to request
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	// 3. fill the creation struct
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;                       // Which window surface
	createInfo.minImageCount = imageCount;              // Number of images
	createInfo.imageFormat = surfaceFormat.format;      // Pixel format (e.g., BGRA8)
	createInfo.imageColorSpace = surfaceFormat.colorSpace; // Color space (e.g., sRGB)
	createInfo.imageExtent = extent;                    // Resolution in pixels
	createInfo.imageArrayLayers = 1;                    // 1 unless stereo 3D
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // We render to these images

	// 4. handle queue family sharing.
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	// Who will access images inside swap chain? One queue family or two different ones?
	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		// Images are used by only one queue family (faster)
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;   
		createInfo.pQueueFamilyIndices = nullptr; 
	}

	// 5. minor settings
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;  
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;            
	createInfo.presentMode = presentMode;                                    
	createInfo.clipped = VK_TRUE;     
	createInfo.oldSwapchain = VK_NULL_HANDLE;                                

	// 6. create the swap chain
	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

	// 7. Retrieve the swap chain images.
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

void cleanupSwapChain(VkDevice device, VkSwapchainKHR swapChain)
{
	vkDestroySwapchainKHR(device, swapChain, nullptr);
}

} // namespace sch