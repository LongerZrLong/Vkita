#pragma once

#include "Common.h"

namespace VKT::Vulkan {

    class Device;

    class ImageView
    {
    public:
        NON_COPIABLE(ImageView);

        explicit ImageView(const Device &device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        ~ImageView();

        const Device &GetDevice() const { return m_Device; }

    private:
        VULKAN_HANDLE(VkImageView, m_VkImageView);

        const Device &m_Device;
        const VkImage m_VkImage;
        const VkFormat m_VkFormat;

    };

}
