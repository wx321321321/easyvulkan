#pragma once
#include "GlfwGeneral.hpp"
#include "EasyVulkan.hpp"
#include <thread>
struct vertex1 {
    glm::vec2 position;
    glm::vec4 color;
};
struct vertex2 {
    glm::vec2 position;
    glm::vec2 texCoord;
};

using namespace vulkan;
pipelineLayout pipelineLayout_triangle;//管线布局
pipeline pipeline_triangle;//管线
descriptorSetLayout descriptorSetLayout_triangle;
descriptorSetLayout descriptorSetLayout_texture;
pipelineLayout pipelineLayout_texture;
pipeline pipeline_texture;//管线
//该函数调用easyVulkan::CreateRpwf_Screen()并存储返回的引用到静态变量，避免重复调用easyVulkan::CreateRpwf_Screen()
const auto& RenderPassAndFramebuffers() {
    static const auto& rpwf = easyVulkan::CreateRpwf_Screen();
    return rpwf;
}
//该函数用于创建管线布局



void CreateLayout() {
   VkDescriptorSetLayoutBinding descriptorSetLayoutBinding_trianglePosition = {
    .binding = 0,
    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,//类型为动态uniform缓冲区
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
    };
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo_triangle = {
        .bindingCount = 1,
        .pBindings = &descriptorSetLayoutBinding_trianglePosition
    };
    descriptorSetLayout_triangle.Create(descriptorSetLayoutCreateInfo_triangle);
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo1 = {
        .setLayoutCount = 1,
        .pSetLayouts = descriptorSetLayout_triangle.Address()
    };
    pipelineLayout_triangle.Create(pipelineLayoutCreateInfo1);
    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding_texture = {
       .binding =1,                                                //描述符被绑定到0号binding
       .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, //类型为带采样器的图像
       .descriptorCount = 1,                                        //个数是1个
       .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT                   //在片段着色器阶段采样图像
    };
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo_texture = {
        .bindingCount = 1,
        .pBindings = &descriptorSetLayoutBinding_texture
    };
    descriptorSetLayout_texture.Create(descriptorSetLayoutCreateInfo_texture);
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo2 = {
        .setLayoutCount = 1,
        .pSetLayouts = descriptorSetLayout_texture.Address()
    };
    pipelineLayout_texture.Create(pipelineLayoutCreateInfo2);
}

//该函数用于创建管线
void CreatePipeline() {
    static shaderModule vert1("shader/1.vert.spv");
    static shaderModule frag1("shader/1.frag.spv");
    static VkPipelineShaderStageCreateInfo shaderStageCreateInfos_triangle[2] = {
        vert1.StageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT),
        frag1.StageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT)
    };
    auto Create1 = [] {
        graphicsPipelineCreateInfoPack pipelineCiPack;
        pipelineCiPack.createInfo.layout = pipelineLayout_triangle;
        pipelineCiPack.createInfo.renderPass = RenderPassAndFramebuffers().renderPass;
        pipelineCiPack.vertexInputBindings.emplace_back(0, sizeof(vertex1), VK_VERTEX_INPUT_RATE_VERTEX);
        //location为0，数据来自0号顶点缓冲区，vec2对应VK_FORMAT_R32G32_SFLOAT
        pipelineCiPack.vertexInputAttributes.emplace_back(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(vertex1, position));
        //location为1，数据来自0号顶点缓冲区，vec4对应VK_FORMAT_R32G32B32A32_SFLOAT
        pipelineCiPack.vertexInputAttributes.emplace_back(1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(vertex1, color));
        pipelineCiPack.inputAssemblyStateCi.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        pipelineCiPack.viewports.emplace_back(0.f, 0.f, float(windowSize.width), float(windowSize.height), 0.f, 1.f);
        pipelineCiPack.scissors.emplace_back(VkOffset2D{}, windowSize);
        pipelineCiPack.multisampleStateCi.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        pipelineCiPack.colorBlendAttachmentStates.push_back({ .colorWriteMask = 0b1111 });
        pipelineCiPack.UpdateAllArrays();
        pipelineCiPack.createInfo.stageCount = 2;
        pipelineCiPack.createInfo.pStages = shaderStageCreateInfos_triangle;

        pipeline_triangle.Create(pipelineCiPack);
        };
    auto Destroy1 = [] {
        pipeline_triangle.~pipeline();
        };
    graphicsBase::Base().AddCallback_CreateSwapchain(Create1);
    graphicsBase::Base().AddCallback_DestroySwapchain(Destroy1);
    Create1();

    static shaderModule vert2("shader/2.vert.spv");
    static shaderModule frag2("shader/2.frag.spv");
    static VkPipelineShaderStageCreateInfo shaderStageCreateInfos_texture[2] = {
        vert2.StageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT),
        frag2.StageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT)
    };
    auto Create2 = [] {
        graphicsPipelineCreateInfoPack pipelineCiPack;
        pipelineCiPack.createInfo.layout = pipelineLayout_texture;
        pipelineCiPack.createInfo.renderPass = RenderPassAndFramebuffers().renderPass;
        pipelineCiPack.vertexInputBindings.emplace_back(0, sizeof(vertex2), VK_VERTEX_INPUT_RATE_VERTEX);
        pipelineCiPack.vertexInputAttributes.emplace_back(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(vertex2, position));
        pipelineCiPack.vertexInputAttributes.emplace_back(1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(vertex2, texCoord));
        pipelineCiPack.inputAssemblyStateCi.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        pipelineCiPack.viewports.emplace_back(0.f, 0.f, float(windowSize.width), float(windowSize.height), 0.f, 1.f);
        pipelineCiPack.scissors.emplace_back(VkOffset2D{}, windowSize);
        pipelineCiPack.multisampleStateCi.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        pipelineCiPack.colorBlendAttachmentStates.push_back({ .colorWriteMask = 0b1111 });
        pipelineCiPack.UpdateAllArrays();
        pipelineCiPack.createInfo.stageCount = 2;
        pipelineCiPack.createInfo.pStages = shaderStageCreateInfos_texture;
        pipeline_texture.Create(pipelineCiPack);
        };
    auto Destroy2 = [] {
        pipeline_texture.~pipeline();
        };
    graphicsBase::Base().AddCallback_CreateSwapchain(Create2);
    graphicsBase::Base().AddCallback_DestroySwapchain(Destroy2);
    Create2();


}

int main() {
    
    if (!InitializeWindow({ 1280,720 }))
        return -1;
    easyVulkan::BootScreen("yuanshen.png", VK_FORMAT_R8G8B8A8_UNORM);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    texture2d texture("yuanshen.png", VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, true);
    if (texture.ImageView() == VK_NULL_HANDLE) {
        std::cerr << "Error: texture image view is VK_NULL_HANDLE!" << std::endl;
        return -1; // 退出或修复代码
    }
    VkSamplerCreateInfo samplerCreateInfo = texture::SamplerCreateInfo();
    sampler sampler(samplerCreateInfo);
    /*变更*/const auto& [renderPass, framebuffers] = RenderPassAndFramebuffers();
    /*新增*/CreateLayout();
    /*新增*/CreatePipeline();
    struct perFrameObjects_t {
        fence fence = { VK_FENCE_CREATE_SIGNALED_BIT }; //直接 = VK_FENCE_CREATE_SIGNALED_BIT也行，这里按照我的编程习惯在初始化类/结构体时保留花括号
        semaphore semaphore_imageIsAvailable;
        semaphore semaphore_renderingIsOver;
        commandBuffer commandBuffer;
    };
    /*新增*/VkClearValue clearColor = { .color = { 1.f, 0.f, 0.f, 1.f } };//红色
    vertex1 vertices1[] = {
   { {  .0f, -.5f }, { 1, 0, 0, 1 } },
   { { -.5f,  .5f }, { 0, 1, 0, 1 } },
   { {  .5f,  .5f }, { 0, 0, 1, 1 } }
    };
    glm::vec2 uniform_positions[] = {
    {  .0f, .0f },
    { -.5f, .0f },
    {  .5f, .0f }
    };
    vertex2 vertices2[] = {
    { { -1.f, -1.f }, { 0, 0 } },
    { {  0.f, -1.f }, { 1, 0 } },
    { { -1.f,  0.f }, { 0, 1 } },
    { {  0.f,  0.f }, { 1, 1 } }
    };
   
    VkDeviceSize uniformAlignment = graphicsBase::Base().PhysicalDeviceProperties().limits.minUniformBufferOffsetAlignment;
    uniformAlignment *= (std::ceil(float(sizeof(glm::vec2)) / uniformAlignment));
    uniformBuffer uniformBuffer(uniformAlignment * 3);
    uniformBuffer.TransferData(uniform_positions, 3, sizeof(glm::vec2), sizeof(glm::vec2), uniformAlignment);
    vertexBuffer vertexBuffer1(sizeof vertices1);
    vertexBuffer1.TransferData(vertices1); 
    vertexBuffer vertexBuffer2(sizeof vertices2);
    vertexBuffer2.TransferData(vertices2);

    VkDescriptorPoolSize descriptorPoolSizes[] = {
     { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 2 },  // 增加需求数量
     { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2 }
    };

  
    descriptorPool descriptorPool(2, descriptorPoolSizes); 
    descriptorSet descriptorSet_trianglePosition;
    descriptorPool.AllocateSets(descriptorSet_trianglePosition, descriptorSetLayout_triangle);
    VkDescriptorBufferInfo bufferInfo = {
        .buffer = uniformBuffer,     // uniform 缓冲区
        .offset = 0,                 // 偏移量
        .range = sizeof(glm::vec2)  // 范围
    };
    descriptorSet_trianglePosition.Write(bufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC);

    
    // 分配描述符集（texture描述符集）
    descriptorSet descriptorSet_texture;
    descriptorPool.AllocateSets(descriptorSet_texture, descriptorSetLayout_texture);
    VkDescriptorImageInfo imageInfo = {
        .sampler = sampler,                        
        .imageView = texture.ImageView(),          
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL 
    };
    // 确保 imageInfo 被正确传递
    descriptorSet_texture.Write(imageInfo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,1);



    std::vector<perFrameObjects_t> perFrameObjects(graphicsBase::Base().SwapchainImageCount());
    commandPool commandPool(graphicsBase::Base().QueueFamilyIndex_Graphics(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    for (auto& i : perFrameObjects)
        commandPool.AllocateBuffers(i.commandBuffer);
    uint32_t frameObjectIndex = 0;

    while (!glfwWindowShouldClose(pWindow)) {
        while (glfwGetWindowAttrib(pWindow, GLFW_ICONIFIED))
            glfwWaitEvents();

        const auto& [fence, semaphore_imageIsAvailable, semaphore_renderingIsOver, commandBuffer] = perFrameObjects[frameObjectIndex];
        frameObjectIndex = (frameObjectIndex + 1) % graphicsBase::Base().SwapchainImageCount();

        fence.WaitAndReset(); //在渲染循环开头等待与当前交换链图像配套的栅栏（在渲染循环末尾等待的话，下一帧必须等待当前帧渲染完，即时帧的做法便没了意义）
        graphicsBase::Base().SwapImage(semaphore_imageIsAvailable);
        auto CurrentImageIndex = graphicsBase::Base().CurrentImageIndex();
        commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        renderPass.CmdBegin(commandBuffer, framebuffers[CurrentImageIndex], { {}, windowSize }, clearColor);
       
        VkDeviceSize offset = 0;
        
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffer1.Address(), &offset);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_triangle);
        for (size_t i = 0; i < 3; i++) {
            uint32_t dynamicOffset = uniformAlignment * i;
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipelineLayout_triangle, 0, 1, descriptorSet_trianglePosition.Address(), 1, &dynamicOffset);
            vkCmdDraw(commandBuffer, 3, 1, 0, 0);
        }

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffer2.Address(), &offset);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_texture);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout_texture, 0, 1, descriptorSet_texture.Address(), 0, nullptr);
        vkCmdDraw(commandBuffer, 4, 1, 0, 0);

        renderPass.CmdEnd(commandBuffer);
        commandBuffer.End();

        graphicsBase::Base().SubmitCommandBuffer_Graphics(commandBuffer, semaphore_imageIsAvailable, semaphore_renderingIsOver, fence);
        graphicsBase::Base().PresentImage(semaphore_renderingIsOver);

        glfwPollEvents();
        TitleFps();
    }
    TerminateWindow();
    return 0;
}