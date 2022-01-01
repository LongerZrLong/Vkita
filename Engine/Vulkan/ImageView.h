#pragma once

#include "Common.h"
#include "Device.h"

namespace VKT::Vulkan {

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
