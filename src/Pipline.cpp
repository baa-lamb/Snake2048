#include "Pipeline.h"

#include <fstream>
#include <array>
namespace 
{

const char* const vertexShader = "shaders/vert.spv";
const char* const fragmentShader = "shaders/frag.spv";

// diff stages of graphics pipeline

// 1.
std::array<VkPipelineShaderStageCreateInfo, 2> createShaderStages(
    VkShaderModule vertShaderModule, VkShaderModule fragShaderModule)
{
    // 1. vertex shader
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main"; // entrypoint (function to invoke)

    // 2. fragment shader
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main"; // entrypoint

    return { vertShaderStageInfo, fragShaderStageInfo };
}

// 2. 
VkPipelineDynamicStateCreateInfo createDynamicState()
{
    static std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>( dynamicStates.size() );
    dynamicState.pDynamicStates = dynamicStates.data();
    return dynamicState;
}

// 3. Vertex Input (now empty)
VkPipelineVertexInputStateCreateInfo createVertexInputState()
{
    VkPipelineVertexInputStateCreateInfo vertexInput{};
    vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    // Bindings: spacing between data and whether the data is per-vertex or per-instance
    vertexInput.vertexBindingDescriptionCount = 0;
    vertexInput.pVertexBindingDescriptions = nullptr;
    vertexInput.vertexAttributeDescriptionCount = 0;
    vertexInput.pVertexAttributeDescriptions = nullptr;
    return vertexInput;
}

// 4. Input Assambly: how vertices are grouped into primitives 
VkPipelineInputAssemblyStateCreateInfo createInputAssemblyState()
{
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    return inputAssembly;
}

// 5. todo (what is this .. )
VkPipelineViewportStateCreateInfo createViewportState()
{
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    return viewportState;
}

// 6. 
VkPipelineRasterizationStateCreateInfo createRasterizer()
{
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;          // Discard fragments beyond near/far
    rasterizer.rasterizerDiscardEnable = VK_FALSE;   // Don't skip rasterization
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;   // Fill triangles, not wireframe
    rasterizer.lineWidth = 1.0f;                     // Line thickness (if line mode)
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;     // Cull back faces
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;  // Clockwise = front face
    rasterizer.depthBiasEnable = VK_FALSE;           // No depth biasing
    return rasterizer;
}

// 7.
// Disabled for now — uses 1 sample per pixel.
VkPipelineMultisampleStateCreateInfo createMultisampling()
{
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    return multisampling;
}

// 8. no blending for now
VkPipelineColorBlendStateCreateInfo createColorBlending()
{
    // Per-framebuffer attachment: no blending, write all color channels
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    // Global color blending settings
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    return colorBlending;
}

// 9.
void createPipelineLayout(VkDevice device, VkPipelineLayout& pipelineLayout)
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

}

std::vector<char> readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    const size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

VkShaderModule createShaderModule(const std::vector<char>& code, VkDevice device)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>( code.data() );

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

void createGraphicsPipeline(
    VkDevice device,
    VkExtent2D swapChainExtent,
    VkPipeline& graphicsPipeline,
    VkPipelineLayout& pipelineLayout)
{
    auto vertShaderCode = readFile(vertexShader);
    auto fragShaderCode = readFile(fragmentShader);

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode, device);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode, device);
    
    auto shaderStages = createShaderStages(vertShaderModule, fragShaderModule);
    auto dynamicState = createDynamicState();
    auto vertexInput = createVertexInputState();
    auto inputAssembly = createInputAssemblyState();
    auto viewportState = createViewportState();
    auto rasterizer = createRasterizer();
    auto multisampling = createMultisampling();
    auto colorBlending = createColorBlending();

    // --- Create pipeline layout (needed for VkPipeline) ---
    createPipelineLayout(device, pipelineLayout);

    // TODO

    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
}