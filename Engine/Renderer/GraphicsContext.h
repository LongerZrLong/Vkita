#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "Core/Base.h"

namespace VKT {

    const int MAX_FRAMES_IN_FLIGHT = 2;

    namespace Vulkan {
        class Instance;
        class DebugUtilsMessenger;
        class Device;
        class Surface;
        class SwapChain;
        class FrameBuffer;
        class DepthBuffer;
        class CommandPool;
        class CommandBuffers;
        class RenderPass;
    }

    class Window;

    class GraphicsContext
    {
    public:
        NON_COPIABLE(GraphicsContext);

        GraphicsContext(Window &window, VkPresentModeKHR presentMode, bool enableValidationLayers);
        ~GraphicsContext();

        const Vulkan::Device &GetDevice() const { return *m_Device; }

        const Vulkan::RenderPass &GetRenderPass() const { return *m_RenderPass; }

        const Vulkan::SwapChain &GetSwapChain() const { return *m_SwapChain; }

        const Vulkan::FrameBuffer &GetFrameBuffer(size_t i) const { return *m_FrameBuffers[i]; };

        const Vulkan::CommandPool &GetCommandPool() const { return *m_CommandPool; }
        const Vulkan::CommandBuffers &GetCommandBuffers() const { return *m_CommandBuffers; }

        const Vulkan::DepthBuffer &GetDepthBuffer() const { return *m_DepthBuffer; }

        uint32_t GetCurrentImageIndex() const { return m_CurrentImageIndex; }

        bool DrawFrameBegin();
        void DrawFrameEnd();

        void DeviceWaitIdle();

    private:
        void CreateDevice();

        void CreateSwapChain();
        void DeleteSwapChain();
        void RecreateSwapChain();

        void CreateSyncObjects();
        void DeleteSyncObjects();

        void LogVulkanInfo();

        const std::vector<VkExtensionProperties> &GetExtensions() const;
        const std::vector<VkLayerProperties> &GetLayers() const;
        const std::vector<VkPhysicalDevice> &GetPhysicalDevices() const;

        void SetPhysicalDevice(VkPhysicalDevice physicalDevice);

    private:
        friend class Renderer;

        Window &m_Window;

        const VkPresentModeKHR m_VkPresentMode;

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

    };
}