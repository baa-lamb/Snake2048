#pragma once
#include <vulkan/vulkan.h>

#include <vector>
#include <string>

std::vector<char> readFile(const std::string& filename);

VkShaderModule createShaderModule(const std::vector<char>& code, VkDevice device);

void createGraphicsPipeline(
    VkDevice device,
    VkExtent2D swapChainExtent, 
    VkRenderPass renderPass,
    VkPipeline& graphicsPipeline,
    VkPipelineLayout& pipelineLayout);