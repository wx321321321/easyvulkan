#include "GlfwGeneral.hpp"
#include "EasyVulkan.hpp"
#include "VKBase+.h"


using namespace vulkan;
pipelineLayout pipelineLayout_triangle;//管线布局
pipeline pipeline_triangle;//管线
//该函数调用easyVulkan::CreateRpwf_Screen()并存储返回的引用到静态变量，避免重复调用easyVulkan::CreateRpwf_Screen()
const auto& RenderPassAndFramebuffers() {
    static const auto& rpwf = easyVulkan::CreateRpwf_Screen();
    return rpwf;
}
//该函数用于创建管线布局
void CreateLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayout_triangle.Create(pipelineLayoutCreateInfo);
}
//该函数用于创建管线
void CreatePipeline() {
    static shaderModule vert("shader/FirstTriangle.vert.spv");
    static shaderModule frag("shader/FirstTriangle.frag.spv");
    static VkPipelineShaderStageCreateInfo shaderStageCreateInfos_triangle[2] = {
        vert.StageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT),
        frag.StageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT)
    };
    auto Create = [] {
        graphicsPipelineCreateInfoPack pipelineCiPack;
        pipelineCiPack.createInfo.layout = pipelineLayout_triangle;
        pipelineCiPack.createInfo.renderPass = RenderPassAndFramebuffers().renderPass;
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
    auto Destroy = [] {
        pipeline_triangle.~pipeline();
        };
    graphicsBase::Base().AddCallback_CreateSwapchain(Create);
    graphicsBase::Base().AddCallback_DestroySwapchain(Destroy);
    Create();
}

int main() {
    if (!InitializeWindow({ 1280,720 }))
        return -1;
   
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
        /*新增，开始渲染通道*/renderPass.CmdBegin(commandBuffer, framebuffers[CurrentImageIndex], { {}, windowSize }, clearColor);
        /*新增*/vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_triangle);
        /*新增*/vkCmdDraw(commandBuffer, 3, 1, 0, 0);
        /*新增，结束渲染通道*/renderPass.CmdEnd(commandBuffer);
        commandBuffer.End();

        graphicsBase::Base().SubmitCommandBuffer_Graphics(commandBuffer, semaphore_imageIsAvailable, semaphore_renderingIsOver, fence);
        graphicsBase::Base().PresentImage(semaphore_renderingIsOver);

        glfwPollEvents();
        TitleFps();
    }
    TerminateWindow();
    return 0;
}