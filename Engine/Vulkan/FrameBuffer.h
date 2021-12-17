#pragma once

#include "Common.h"

namespace VKT::Vulkan {

    class ImageView;
    class RenderPass;

    class FrameBuffer
    {
    public:
        FrameBuffer(const FrameBuffer&) = delete;
        FrameBuffer &operator = (const FrameBuffer&) = delete;
        FrameBuffer &operator = (FrameBuffer&&) = delete;

        explicit FrameBuffer(const ImageView &imageView, const RenderPass &renderPass);
        FrameBuffer(FrameBuffer &&other) noexcept;
        ~FrameBuffer();

        const ImageView &GetImageView() const { return m_ImageView; }
        const RenderPass &GetRenderPass() const { return m_RenderPass; }

    private:
        VULKAN_HANDLE(VkFramebuffer, m_VkFramebuffer);

        const ImageView &m_ImageView;
        const RenderPass &m_RenderPass;
    };

}
