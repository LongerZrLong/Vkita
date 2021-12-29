#include "FrameBuffer.h"

#include <array>

#include "ImageView.h"
#include "DepthBuffer.h"
#include "SwapChain.h"

namespace VKT::Vulkan {

    FrameBuffer::FrameBuffer(const Device &device, const ImageView &imageView, const RenderPass &renderPass)
        : m_Device(device), m_ImageView(imageView), m_RenderPass(renderPass)
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

        Check(vkCreateFramebuffer(device.GetVkHandle(), &framebufferInfo, nullptr, &m_VkFramebuffer));
    }

    FrameBuffer::FrameBuffer(FrameBuffer &&other) noexcept
    : m_Device(other.m_Device),
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
            vkDestroyFramebuffer(m_Device.GetVkHandle(), m_VkFramebuffer, nullptr);
            m_VkFramebuffer = nullptr;
        }
    }

}
