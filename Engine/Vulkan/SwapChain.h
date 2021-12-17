#pragma once

#include "Common.h"

#include <memory>
#include <vector>

namespace VKT {
    class Window;
}

namespace VKT::Vulkan {

    class Device;
    class ImageView;

    class SwapChain
    {
    public:
        NON_COPIABLE(SwapChain);

        SwapChain(const Device &device, VkPresentModeKHR presentMode);
        ~SwapChain();

        VkPhysicalDevice GetVkPhysicalDevice() const { return m_VkPhysicalDevice; }
        const Device &GetDevice() const { return m_Device; }
        uint32_t GetMinImageCount() const { return m_MinImageCount; }
        const std::vector<VkImage> &GetVkImages() const { return m_VkImages; }
        const std::vector<std::unique_ptr<ImageView>> &GetImageViews() const { return m_ImageViews; }
        const VkExtent2D &GetVkExtent2D() const { return m_VkExtent2D; }
        VkFormat GetVkFormat() const { return m_VkFormat; }
        VkPresentModeKHR GetVkPresentMode() const { return m_VkPresentMode; }

    private:
        struct SupportDetails
        {
            VkSurfaceCapabilitiesKHR Capabilities{};
            std::vector<VkSurfaceFormatKHR> Formats;
            std::vector<VkPresentModeKHR> PresentModes;
        };

        static SupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

        static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats);
        static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &presentModes, VkPresentModeKHR presentMode);
        static VkExtent2D ChooseSwapExtent(const Window& window, const VkSurfaceCapabilitiesKHR &capabilities);
        static uint32_t ChooseImageCount(const VkSurfaceCapabilitiesKHR &capabilities);

    private:
        VULKAN_HANDLE(VkSwapchainKHR, m_VkSwapchain);

        const VkPhysicalDevice m_VkPhysicalDevice;
        const Device &m_Device;

        uint32_t m_MinImageCount;
        VkPresentModeKHR m_VkPresentMode;
        VkFormat m_VkFormat;
        VkExtent2D m_VkExtent2D{};
        std::vector<VkImage> m_VkImages;
        std::vector<Scope<ImageView>> m_ImageViews;
    };

}
