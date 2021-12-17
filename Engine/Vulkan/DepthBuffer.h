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

        DepthBuffer(CommandPool &commandPool, VkExtent2D extent);
        ~DepthBuffer();

        VkFormat Format() const { return m_VkFormat; }
        const ImageView &GetImageView() const { return *m_ImageView; }

        static bool HasStencilComponent(const VkFormat format)
        {
            return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
        }

    private:
        const VkFormat m_VkFormat;
        std::unique_ptr<Image> m_Image;
        std::unique_ptr<DeviceMemory> m_DeviceMemory;
        std::unique_ptr<ImageView> m_ImageView;

    };

}
