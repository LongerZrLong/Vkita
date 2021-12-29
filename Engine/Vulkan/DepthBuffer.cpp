#include "DepthBuffer.h"

#include "Core/Base.h"

#include "CommandPool.h"
#include "Device.h"
#include "DeviceMemory.h"
#include "Image.h"
#include "ImageView.h"

namespace VKT::Vulkan {

    namespace
    {
        VkFormat FindSupportedFormat(const Device &device, const std::vector<VkFormat> &candidates, const VkImageTiling tiling, const VkFormatFeatureFlags features)
        {
            for (auto format : candidates)
            {
                VkFormatProperties props;
                vkGetPhysicalDeviceFormatProperties(device.GetVkPhysicalDevice(), format, &props);

                if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
                {
                    return format;
                }

                if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
                {
                    return format;
                }
            }

            throw std::runtime_error("failed to find supported format");
        }

        VkFormat FindDepthFormat(const Device &device)
        {
            return FindSupportedFormat(
                device,
                { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
                VK_IMAGE_TILING_OPTIMAL,
                VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
            );
        }
    }

    DepthBuffer::DepthBuffer(const Device &device, const CommandPool &commandPool, const VkExtent2D extent)
        : m_VkFormat(FindDepthFormat(device))
    {
        m_Image = CreateScope<Image>(device, extent, m_VkFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
        m_DeviceMemory = CreateScope<DeviceMemory>(m_Image->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
        m_ImageView = CreateScope<ImageView>(device, m_Image->GetVkHandle(), m_VkFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

        m_Image->TransitionImageLayout(device, commandPool, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

        const auto &debugUtils = device.GetDebugUtils();

        debugUtils.SetObjectName(m_Image->GetVkHandle(), "Depth Buffer Image");
        debugUtils.SetObjectName(m_DeviceMemory->GetVkHandle(), "Depth Buffer Image Memory");
        debugUtils.SetObjectName(m_ImageView->GetVkHandle(), "Depth Buffer ImageView");
    }

    DepthBuffer::~DepthBuffer()
    {
        m_ImageView.reset();
        m_Image.reset();
        m_DeviceMemory.reset();
    }

}
