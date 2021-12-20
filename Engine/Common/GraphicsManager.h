#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "Common/Base.h"

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

// Temporary
#include "Renderer/GraphicsPipeline.h"
#include "Renderer/Buffer.h"
#include "Renderer/Texture2D.h"
#include "Renderer/UniformBuffer.h"
#include "Renderer/DescriptorBinding.h"

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
        friend class Renderer;

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
        Ref<GraphicsPipeline> m_GraphicsPipeline;

        Ref<Shader> m_VertShader;
        Ref<Shader> m_FragShader;

        Ref<VertexBuffer> m_VertexBuffer;
        Ref<IndexBuffer> m_IndexBuffer;

        Ref<DescriptorSetManager> m_DescriptorSetManager;
        Ref<UniformBuffer> m_UniformBuffer;
        Ref<Texture2D> m_CheckerBoardTex;

    };

    extern GraphicsManager *g_GraphicsManager;

}