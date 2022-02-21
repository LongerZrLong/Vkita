#pragma once

#include <vector>

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

namespace VKT {
    class GraphicsManager;
}

namespace VKT::Rendering {

    class Context
    {
    public:
        const VKT::Vulkan::Device &GetDevice() const { return *device; }
        const VKT::Vulkan::SwapChain &GetSwapChain() const { return *swapChain; }
        const VKT::Vulkan::CommandPool &GetCommandPool() const { return *cmdPool; }
        const VKT::Vulkan::RenderPass &GetRenderPass() const { return *renderPass; }

    private:
        void Initialize();
        void ShutDown();
        void RecreateSwapChain();

        const std::vector<VkExtensionProperties> &GetExtensions() const;
        const std::vector<VkLayerProperties> &GetLayers() const;
        const std::vector<VkPhysicalDevice> &GetPhysicalDevices() const;

    private:
        void CreateDevice();

        void CreateSwapChain();
        void DeleteSwapChain();

        void CreateSyncObjects();
        void DeleteSyncObjects();

        void SetPhysicalDevice(VkPhysicalDevice physicalDevice);

        void LogVulkanInfo();

    private:
        friend class VKT::GraphicsManager;

        Scope<VKT::Vulkan::Instance> instance;
        Scope<VKT::Vulkan::DebugUtilsMessenger> debugUtilsMessenger;
        Scope<VKT::Vulkan::Surface> surface;
        Scope<VKT::Vulkan::Device> device;

        Scope<VKT::Vulkan::SwapChain> swapChain;

        std::vector<VKT::Vulkan::FrameBuffer> frameBuffers;

        Scope<VKT::Vulkan::RenderPass> renderPass;
        Scope<VKT::Vulkan::DepthBuffer> depthBuffer;

        Scope<VKT::Vulkan::CommandPool> cmdPool;
        Scope<VKT::Vulkan::CommandBuffers> cmdBuffers;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;

        size_t currentFrame{};
        uint32_t currentImageIndex{};

    };
}