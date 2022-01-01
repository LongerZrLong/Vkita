#pragma once

#include "Common.h"
#include "Device.h"
#include "SwapChain.h"
#include "DepthBuffer.h"

namespace VKT::Vulkan {

    class DepthBuffer;
    class SwapChain;

    class RenderPass
    {
    public:
        NON_COPIABLE(RenderPass);

        RenderPass(const Device &device, const SwapChain &swapChain, const DepthBuffer &depthBuffer, VkAttachmentLoadOp colorBufferLoadOp, VkAttachmentLoadOp depthBufferLoadOp);
        ~RenderPass();

        const SwapChain &GetSwapChain() const { return m_SwapChain; }
        const DepthBuffer &GetDepthBuffer() const { return m_DepthBuffer; }

    private:
        VULKAN_HANDLE(VkRenderPass, m_VkRenderPass);

        const Device &m_Device;
        const SwapChain &m_SwapChain;
        const DepthBuffer &m_DepthBuffer;

    };

}
