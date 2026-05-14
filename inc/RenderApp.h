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
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;

    void initVulkan();
    void createInst();
    void setupDebugMessenger();
    void mainLoop();
    void cleanup();

    // ---- glfw
    GLFWwindow* window;
    void initWindow();
};

} // namespace rn