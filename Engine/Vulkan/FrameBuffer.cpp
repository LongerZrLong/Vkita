#include "FrameBuffer.h"

#include <array>

#include "DepthBuffer.h"
#include "Device.h"
#include "ImageView.h"
#include "RenderPass.h"
#include "SwapChain.h"

namespace VKT::Vulkan {

    FrameBuffer::FrameBuffer(const ImageView &imageView, const RenderPass &renderPass)
        : m_ImageView(imageView), m_RenderPass(renderPass)
    {
        std::array<VkImageView, 2> attachments =
            {
                imageView.GetVkHandle(),
                renderPass.GetDepthBuffer().GetImageView().GetVkHandle()
            };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass.GetVkHandle();
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = renderPass.GetSwapChain().GetVkExtent2D().width;
        framebufferInfo.height = renderPass.GetSwapChain().GetVkExtent2D().height;
        framebufferInfo.layers = 1;

        Check(vkCreateFramebuffer(m_ImageView.GetDevice().GetVkHandle(), &framebufferInfo, nullptr, &m_VkFramebuffer),
              "create framebuffer");
    }

    FrameBuffer::FrameBuffer(FrameBuffer &&other) noexcept :
        m_ImageView(other.m_ImageView),
        m_RenderPass(other.m_RenderPass),
        m_VkFramebuffer(other.m_VkFramebuffer)
    {
        other.m_VkFramebuffer = nullptr;
    }

    FrameBuffer::~FrameBuffer()
    {
        if (m_VkFramebuffer != nullptr)
        {
            vkDestroyFramebuffer(m_ImageView.GetDevice().GetVkHandle(), m_VkFramebuffer, nullptr);
            m_VkFramebuffer = nullptr;
        }
    }

}
