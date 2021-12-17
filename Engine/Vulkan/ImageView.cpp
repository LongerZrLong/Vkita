#include "ImageView.h"

#include "Device.h"

namespace VKT::Vulkan {

    ImageView::ImageView(const Device &device, const VkImage image, const VkFormat format, const VkImageAspectFlags aspectFlags)
        : m_Device(device), m_VkImage(image), m_VkFormat(format)
    {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = image;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = aspectFlags;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        Check(vkCreateImageView(m_Device.GetVkHandle(), &createInfo, nullptr, &m_VkImageView),
              "create image view");
    }

    ImageView::~ImageView()
    {
        if (m_VkImageView != nullptr)
        {
            vkDestroyImageView(m_Device.GetVkHandle(), m_VkImageView, nullptr);
            m_VkImageView = nullptr;
        }
    }

}