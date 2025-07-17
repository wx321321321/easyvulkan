#include "GlfwGeneral.hpp"
#include "EasyVulkan.hpp"

using namespace vulkan;
int main() {
    if (!InitializeWindow({ 1280,720 }))
        return -1;
    const auto& [renderPass, framebuffers] = easyVulkan::CreateRpwf_Screen();
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
    uint32_t i = 0;

    while (!glfwWindowShouldClose(pWindow)) {
        while (glfwGetWindowAttrib(pWindow, GLFW_ICONIFIED))
            glfwWaitEvents();

        const auto& [fence, semaphore_imageIsAvailable, semaphore_renderingIsOver, commandBuffer] = perFrameObjects[i];
        i = (i + 1) % graphicsBase::Base().SwapchainImageCount();

        fence.WaitAndReset(); //在渲染循环开头等待与当前交换链图像配套的栅栏（在渲染循环末尾等待的话，下一帧必须等待当前帧渲染完，即时帧的做法便没了意义）
        graphicsBase::Base().SwapImage(semaphore_imageIsAvailable);
        auto i = graphicsBase::Base().CurrentImageIndex();
        commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        /*新增，开始渲染通道*/renderPass.CmdBegin(commandBuffer, framebuffers[i], { {}, windowSize }, clearColor);
        /*渲染命令，待填充*/
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