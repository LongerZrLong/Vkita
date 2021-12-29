#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "Core/Base.h"

#include "Interface/IRuntimeModule.h"

#include "Vulkan/Instance.h"
#include "Vulkan/DebugUtilsMessenger.h"
#include "Vulkan/Device.h"
#include "Vulkan/Surface.h"
#include "Vulkan/SwapChain.h"
#include "Vulkan/FrameBuffer.h"
#include "Vulkan/DepthBuffer.h"
#include "Vulkan/CommandPool.h"
#include "Vulkan/CommandBuffers.h"
#include "Vulkan/RenderPass.h"
#include "Vulkan/Enumerate.h"
#include "Vulkan/Version.h"
#include "Vulkan/Strings.h"
#include "Vulkan/DescriptorPool.h"
#include "Vulkan/DescriptorSetLayout.h"
#include "Vulkan/DescriptorSet.h"
#include "Vulkan/Pipeline.h"
#include "Vulkan/PipelineLayout.h"
#include "Vulkan/ShaderModule.h"

// Temporary
#include "Renderer/VulkanBuffer.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Texture2D.h"

namespace VKT {

    class GraphicsManager : public IRuntimeModule
    {
    public:
        GraphicsManager() = default;
        ~GraphicsManager() override = default;

        int Initialize() override;
        void ShutDown() override;

        void Tick() override;

        void Draw();

        const Vulkan::Device &GetDevice() const { return *m_Device; }
        const Vulkan::SwapChain &GetSwapChain() const { return *m_SwapChain; }
        const Vulkan::CommandPool &GetCommandPool() const { return *m_CommandPool; }
        const Vulkan::RenderPass &GetRenderPass() const { return *m_RenderPass; }

    private:
        bool BeginFrame();
        void EndFrame();

        void Present();

        void DeviceWaitIdle();

        void LogVulkanInfo();

    private:
        void CreateDevice();

        void CreateSwapChain();
        void DeleteSwapChain();
        void RecreateSwapChain();

        void CreateSyncObjects();
        void DeleteSyncObjects();

        const std::vector<VkExtensionProperties> &GetExtensions() const;
        const std::vector<VkLayerProperties> &GetLayers() const;
        const std::vector<VkPhysicalDevice> &GetPhysicalDevices() const;

        void SetPhysicalDevice(VkPhysicalDevice physicalDevice);

    private:
        Scope<Vulkan::Instance> m_Instance;
        Scope<Vulkan::DebugUtilsMessenger> m_DebugUtilsMessenger;
        Scope<Vulkan::Surface> m_Surface;
        Scope<Vulkan::Device> m_Device;

        Scope<Vulkan::SwapChain> m_SwapChain;

        std::vector<Scope<Vulkan::FrameBuffer>> m_FrameBuffers;

        Scope<Vulkan::RenderPass> m_RenderPass;
        Scope<Vulkan::DepthBuffer> m_DepthBuffer;

        Scope<Vulkan::CommandPool> m_CommandPool;
        Scope<Vulkan::CommandBuffers> m_CommandBuffers;

        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        std::vector<VkFence> m_InFlightFences;
        std::vector<VkFence> m_ImagesInFlight;

        size_t m_CurrentFrame{};
        uint32_t m_CurrentImageIndex{};

        // TODO: Delete Testing Code
        Ref<Vulkan::DescriptorPool> m_DescriptorPool;
        Ref<Vulkan::DescriptorSetLayout> m_DescriptorSetLayout;
        Ref<Vulkan::DescriptorSet> m_DescriptorSet;

        Ref<Vulkan::PipelineLayout> m_PipelineLayout;
        Ref<Vulkan::Pipeline> m_GraphicsPipeline;

        Ref<Vulkan::ShaderModule> m_VertShader;
        Ref<Vulkan::ShaderModule> m_FragShader;

        Ref<VertexBuffer> m_VertexBuffer;
        Ref<IndexBuffer> m_IndexBuffer;

        Ref<VulkanBuffer> m_UniformBuffer;
        Ref<Texture2D> m_CheckerBoardTex;

    };

    extern GraphicsManager *g_GraphicsManager;

}