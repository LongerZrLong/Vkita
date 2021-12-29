#pragma once

#include "Common.h"
#include "Device.h"

namespace VKT::Vulkan {

    class DescriptorSetLayout
    {
    public:
        NON_COPIABLE(DescriptorSetLayout);

        DescriptorSetLayout(const Device &device, VkDescriptorSetLayoutCreateInfo *layoutInfo);
        ~DescriptorSetLayout();

    private:
        VULKAN_HANDLE(VkDescriptorSetLayout, m_VkDescriptorSetLayout);

        const Device & m_Device;

    };

}
