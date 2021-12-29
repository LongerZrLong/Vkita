#pragma once

#include <memory>

#include "Common.h"

namespace VKT::Vulkan {

    class CommandPool;
    class Device;
    class DeviceMemory;
    class Image;
    class ImageView;

    class DepthBuffer
    {
    public:
        NON_COPIABLE(DepthBuffer);

        DepthBuffer(const Device &device, const CommandPool &commandPool, VkExtent2D extent);
        ~DepthBuffer();

        VkFormat Format() const { return m_VkFormat; }
        const ImageView &GetImageView() const { return *m_ImageView; }

        static bool HasStencilComponent(const VkFormat format)
        {
            return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
        }

    private:
        const VkFormat m_VkFormat;
        Scope<Image> m_Image;
        Scope<DeviceMemory> m_DeviceMemory;
        Scope<ImageView> m_ImageView;

    };

}
