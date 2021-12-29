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

    private:
        VULKAN_HANDLE(VkImageView, m_VkImageView);

        const Device &m_Device;

    };

}
