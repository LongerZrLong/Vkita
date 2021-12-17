#pragma once

#include "Common.h"

namespace VKT::Vulkan {

    class DepthBuffer;
    class SwapChain;

    class RenderPass
    {
    public:
        NON_COPIABLE(RenderPass);

        RenderPass(const SwapChain &swapChain, const DepthBuffer &depthBuffer, VkAttachmentLoadOp colorBufferLoadOp, VkAttachmentLoadOp depthBufferLoadOp);
        ~RenderPass();

        const SwapChain &GetSwapChain() const { return m_SwapChain; }
        const DepthBuffer &GetDepthBuffer() const { return m_DepthBuffer; }

    private:
        VULKAN_HANDLE(VkRenderPass, m_VkRenderPass);

        const SwapChain &m_SwapChain;
        const DepthBuffer &m_DepthBuffer;

    };

}
