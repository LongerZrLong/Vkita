#pragma once

#include "Common.h"
#include "Device.h"
#include "RenderPass.h"
#include "ImageView.h"

namespace VKT::Vulkan {

    class FrameBuffer
    {
    public:
        FrameBuffer(const FrameBuffer&) = delete;
        FrameBuffer &operator = (const FrameBuffer&) = delete;
        FrameBuffer &operator = (FrameBuffer&&) = delete;

        FrameBuffer(const Device &device, const ImageView &imageView, const RenderPass &renderPass);
        FrameBuffer(FrameBuffer &&other) noexcept;
        ~FrameBuffer();

        const ImageView &GetImageView() const { return m_ImageView; }
        const RenderPass &GetRenderPass() const { return m_RenderPass; }

    private:
        VULKAN_HANDLE(VkFramebuffer, m_VkFramebuffer);

        const Device &m_Device;
        const ImageView &m_ImageView;
        const RenderPass &m_RenderPass;
    };

}
