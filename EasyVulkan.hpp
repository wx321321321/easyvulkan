#include "VKBase.h"


using namespace vulkan;
const VkExtent2D& windowSize = graphicsBase::Base().SwapchainCreateInfo().imageExtent;

namespace easyVulkan {
    using namespace vulkan;
    struct renderPassWithFramebuffers {
        renderPass renderPass;
        std::vector<framebuffer> framebuffers;
    };



    const auto& CreateRpwf_Screen() {
        static renderPassWithFramebuffers rpwf;

        VkAttachmentDescription attachmentDescription = {
         .format = graphicsBase::Base().SwapchainCreateInfo().imageFormat,
         .samples = VK_SAMPLE_COUNT_1_BIT,
         .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
         .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
         .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
          .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        };
        VkAttachmentReference attachmentReference = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
        VkSubpassDescription subpassDescription = {
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = &attachmentReference
        };
        VkSubpassDependency subpassDependency = {
          .srcSubpass = VK_SUBPASS_EXTERNAL,
         .dstSubpass = 0,
         .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
          .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,//�������ύ�������ʱ�ȴ�semaphore��Ӧ��waitDstStageMask
          .srcAccessMask = 0,
           .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
         .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
        };
        VkRenderPassCreateInfo renderPassCreateInfo = {
            .attachmentCount = 1,
             .pAttachments = &attachmentDescription,
            .subpassCount = 1,
            .pSubpasses = &subpassDescription,
            .dependencyCount = 1,
             .pDependencies = &subpassDependency
        };
        rpwf.renderPass.Create(renderPassCreateInfo);
        rpwf.framebuffers.resize(graphicsBase::Base().SwapchainImageCount());
        VkFramebufferCreateInfo framebufferCreateInfo = {
            .renderPass = rpwf.renderPass,
             .attachmentCount = 1,
            .width = windowSize.width,
             .height = windowSize.height,
            .layers = 1
        };
        for (size_t i = 0; i < graphicsBase::Base().SwapchainImageCount(); i++) {
            VkImageView attachment = graphicsBase::Base().SwapchainImageView(i);
            framebufferCreateInfo.pAttachments = &attachment;
            rpwf.framebuffers[i].Create(framebufferCreateInfo);
        }
        auto CreateFramebuffers = [] {
            rpwf.framebuffers.resize(graphicsBase::Base().SwapchainImageCount());
            VkFramebufferCreateInfo framebufferCreateInfo = {
                .renderPass = rpwf.renderPass,
                .attachmentCount = 1,
                .width = windowSize.width,
                .height = windowSize.height,
                .layers = 1
            };
            for (size_t i = 0; i < graphicsBase::Base().SwapchainImageCount(); i++) {
                VkImageView attachment = graphicsBase::Base().SwapchainImageView(i);
                framebufferCreateInfo.pAttachments = &attachment;
                rpwf.framebuffers[i].Create(framebufferCreateInfo);
            }
            };
        auto DestroyFramebuffers = [] {
            rpwf.framebuffers.clear();//���vector�е�Ԫ��ʱ����һִ����������
            };
        graphicsBase::Base().AddCallback_CreateSwapchain(CreateFramebuffers);
        graphicsBase::Base().AddCallback_DestroySwapchain(DestroyFramebuffers);

        return rpwf;
    }

}