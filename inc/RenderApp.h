#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "DebugUtils.h"

namespace rn
{

class RenderApplication {
public:
    void run();

private:
    // ---- vulkan
    VkInstance instance;
    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE; // logical device

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    VkDebugUtilsMessengerEXT debugMessenger;

    void initVulkan();
    void createInst();
    void createSurface();
    void setupDebugMessenger();
    void createLogicalDevice();
    void mainLoop();
    void cleanup();

    // ---- glfw
    GLFWwindow* window;
    void initWindow();
};

} // namespace rn