#include "SwapChain.h"

#include <algorithm>
#include <limits>
#include <string>

#include "Enumerate.h"
#include "Surface.h"

namespace VKT::Vulkan {

    SwapChain::SwapChain(const Device& device, const VkPresentModeKHR presentMode)
        : m_VkPhysicalDevice(device.GetVkPhysicalDevice()), m_Device(device)
    {
        const auto details = QuerySwapChainSupport(device.GetVkPhysicalDevice(), device.GetSurface().GetVkHandle());
        if (details.Formats.empty() || details.PresentModes.empty())
        {
            throw std::runtime_error("empty swap chain support");
        }

        const auto& surface = device.GetSurface();
        const auto& window = surface.GetInstance().GetWindow();

        const auto surfaceFormat = ChooseSwapSurfaceFormat(details.Formats);
        const auto actualPresentMode = ChooseSwapPresentMode(details.PresentModes, presentMode);
        const auto extent = ChooseSwapExtent(window, details.Capabilities);
        const auto imageCount = ChooseImageCount(details.Capabilities);

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface.GetVkHandle();
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        createInfo.preTransform = details.Capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = actualPresentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = nullptr;

        if (device.GetGraphicsFamilyIndex() != device.GetPresentFamilyIndex())
        {
            uint32_t queueFamilyIndices[] = { device.GetGraphicsFamilyIndex(), device.GetPresentFamilyIndex() };

            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        Check(vkCreateSwapchainKHR(device.GetVkHandle(), &createInfo, nullptr, &m_VkSwapchain));

        m_MinImageCount = details.Capabilities.minImageCount;
        m_VkPresentMode = actualPresentMode;
        m_VkFormat = surfaceFormat.format;
        m_VkExtent2D = extent;
        m_VkImages = GetEnumerateVector(m_Device.GetVkHandle(), m_VkSwapchain, vkGetSwapchainImagesKHR);
        m_ImageViews.reserve(m_VkImages.size());

        for (const auto image : m_VkImages)
        {
            m_ImageViews.emplace_back(CreateScope<ImageView>(device, image, m_VkFormat, VK_IMAGE_ASPECT_COLOR_BIT));
        }

        const auto &debugUtils = device.GetDebugUtils();

        for (size_t i = 0; i != m_VkImages.size(); ++i)
        {
            debugUtils.SetObjectName(m_VkImages[i], ("Swapchain Image #" + std::to_string(i)).c_str());
            debugUtils.SetObjectName(m_ImageViews[i]->GetVkHandle(), ("Swapchain ImageView #" + std::to_string(i)).c_str());
        }
    }

    SwapChain::~SwapChain()
    {
        m_ImageViews.clear();

        if (m_VkSwapchain != nullptr)
        {
            vkDestroySwapchainKHR(m_Device.GetVkHandle(), m_VkSwapchain, nullptr);
            m_VkSwapchain = nullptr;
        }
    }

    VkResult SwapChain::AcquireNextImage(VkSemaphore presentCompleteSemaphore, uint32_t *imageIndex)
    {
        return vkAcquireNextImageKHR(m_Device.GetVkHandle(), m_VkSwapchain, UINT64_MAX, presentCompleteSemaphore, VK_NULL_HANDLE, imageIndex);
    }

    VkResult SwapChain::QueuePresent(VkQueue presentQueue, uint32_t imageIndex, VkSemaphore waitSemaphore)
    {
        VkSemaphore waitSemaphores[] = { waitSemaphore };

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = waitSemaphores;

        VkSwapchainKHR swapChains[] = { m_VkSwapchain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        return vkQueuePresentKHR(presentQueue, &presentInfo);
    }

    SwapChain::SupportDetails SwapChain::QuerySwapChainSupport(VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface)
    {
        SupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.Capabilities);
        details.Formats = GetEnumerateVector(physicalDevice, surface, vkGetPhysicalDeviceSurfaceFormatsKHR);
        details.PresentModes = GetEnumerateVector(physicalDevice, surface, vkGetPhysicalDeviceSurfacePresentModesKHR);

        return details;
    }

    VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats)
    {
        if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
        {
            return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        }

        for (const auto &format : formats)
        {
            if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return format;
            }
        }

        throw std::runtime_error("found no suitable surface format");
    }

    VkPresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &presentModes, const VkPresentModeKHR presentMode)
    {
        // VK_PRESENT_MODE_IMMEDIATE_KHR specifies that the presentation engine does not wait for a vertical blanking period
        // to update the current image, meaning this mode may result in visible tearing. No internal queuing of presentation
        // requests is needed, as the requests are applied immediately.

        // VK_PRESENT_MODE_MAILBOX_KHR specifies that the presentation engine waits for the next vertical blanking period to
        // update the current image. Tearing cannot be observed. An internal single-entry queue is used to hold pending
        // presentation requests. If the queue is full when a new presentation request is received, the new request replaces
        // the existing entry, and any images associated with the prior entry become available for re-use by the application.
        // One request is removed from the queue and processed during each vertical blanking period in which the queue is non-empty.

        // VK_PRESENT_MODE_FIFO_KHR specifies that the presentation engine waits for the next vertical blanking period to update
        // the current image. Tearing cannot be observed. An internal queue is used to hold pending presentation requests.
        // New requests are appended to the end of the queue, and one request is removed from the beginning of the queue and
        // processed during each vertical blanking period in which the queue is non-empty. This is the only value of presentMode
        // that is required to be supported.

        // VK_PRESENT_MODE_FIFO_RELAXED_KHR specifies that the presentation engine generally waits for the next vertical blanking
        // period to update the current image. If a vertical blanking period has already passed since the last update of the current
        // image then the presentation engine does not wait for another vertical blanking period for the update, meaning this mode
        // may result in visible tearing in this case. This mode is useful for reducing visual stutter with an application that will
        // mostly present a new image before the next vertical blanking period, but may occasionally be late, and present a new
        // image just after the next vertical blanking period. An internal queue is used to hold pending presentation requests.
        // New requests are appended to the end of the queue, and one request is removed from the beginning of the queue and
        // processed during or after each vertical blanking period in which the queue is non-empty.


        switch (presentMode)
        {
            case VK_PRESENT_MODE_IMMEDIATE_KHR:
            case VK_PRESENT_MODE_MAILBOX_KHR:
            case VK_PRESENT_MODE_FIFO_KHR:
            case VK_PRESENT_MODE_FIFO_RELAXED_KHR:

                if (std::find(presentModes.begin(), presentModes.end(), presentMode) != presentModes.end())
                {
                    return presentMode;
                }

                break;

            default:
                throw std::out_of_range("unknown present mode");
        }

        // Fallback
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D SwapChain::ChooseSwapExtent(const Window &window, const VkSurfaceCapabilitiesKHR &capabilities)
    {
        // Vulkan tells us to match the resolution of the window by setting the width and height in the currentExtent member.
        // However, some window managers do allow us to differ here and this is indicated by setting the width and height in
        // currentExtent to a special value: the maximum value of uint32_t. In that case we'll pick the resolution that best
        // matches the window within the minImageExtent and maxImageExtent bounds.
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }

        VkExtent2D actualExtent = { window.GetWidth(), window.GetHeight() };

        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }

    uint32_t SwapChain::ChooseImageCount(const VkSurfaceCapabilitiesKHR &capabilities)
    {
        uint32_t imageCount = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
        {
            imageCount = capabilities.maxImageCount;
        }

        return imageCount;
    }

}
