#include "Renderer.h"

#include "Vulkan/RenderPass.h"
#include "Vulkan/SwapChain.h"
#include "Vulkan/FrameBuffer.h"
#include "Vulkan/Pipeline.h"
#include "Vulkan/CommandBuffers.h"
#include "Vulkan/Buffer.h"

namespace VKT {

    GraphicsContext *Renderer::s_GraphicsContext = nullptr;

    void Renderer::Init(GraphicsContext *graphicsContext)
    {
        s_GraphicsContext = graphicsContext;
    }

    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        s_GraphicsContext->RecreateSwapChain();
    }

    void Renderer::BeginScene()
    {

    }

    void Renderer::EndScene()
    {

    }

    void Renderer::Draw(const Ref<GraphicsPipeline> &graphicsPipeline,
                        const Ref<VertexBuffer> &vertexBuffer,
                        const Ref<IndexBuffer> &indexBuffer,
                        const Ref<DescriptorSetManager> &descriptorSetManager)
    {
        uint32_t imageIndex = s_GraphicsContext->GetCurrentImageIndex();

        const Vulkan::CommandBuffers &commandBuffers = s_GraphicsContext->GetCommandBuffers();
        const Vulkan::SwapChain &swapChain = s_GraphicsContext->GetSwapChain();
        const Vulkan::RenderPass &renderPass = s_GraphicsContext->GetRenderPass();
        const Vulkan::FrameBuffer &frameBuffers = s_GraphicsContext->GetFrameBuffer(imageIndex);

        VkCommandBuffer vkCommandBuffer = commandBuffers.Begin(imageIndex);

        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = renderPass.GetVkHandle();
        renderPassBeginInfo.framebuffer = frameBuffers.GetVkHandle();
        renderPassBeginInfo.renderArea.offset = {0, 0};
        renderPassBeginInfo.renderArea.extent = swapChain.GetVkExtent2D();

        // TODO: Expose clear value so that they can be set via Renderer API
        std::array<VkClearValue, 2> clearValues = {};
        clearValues[0].color = {{0.1f, 0.1f, 0.1f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassBeginInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(vkCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Draw Call
        {
            vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->GetPipeline().GetVkHandle());

            VkBuffer vertexBuffers[] = { vertexBuffer->GetBuffer().GetVkHandle() };
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(vkCommandBuffer, 0, 1, vertexBuffers, offsets);

            vkCmdBindIndexBuffer(vkCommandBuffer, indexBuffer->GetBuffer().GetVkHandle(), 0, VK_INDEX_TYPE_UINT32);

            VkDescriptorSet descriptorSets[] = { descriptorSetManager->GetDescriptorSet().GetVkHandle() };
            vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    graphicsPipeline->GetPipelineLayout().GetVkHandle(), 0, 1,
                                    descriptorSets, 0, nullptr);

            vkCmdDrawIndexed(vkCommandBuffer, indexBuffer->GetCount(), 1, 0, 0, 0);
        }

        vkCmdEndRenderPass(vkCommandBuffer);

        commandBuffers.End(imageIndex);
    }
}