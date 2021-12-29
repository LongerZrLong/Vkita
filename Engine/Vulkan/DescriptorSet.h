#pragma once

#include "Common.h"
#include "Device.h"

namespace VKT::Vulkan {

    class DescriptorSet
    {
    public:
        NON_COPIABLE(DescriptorSet);

        DescriptorSet(const Device &device, VkDescriptorSetAllocateInfo *allocInfo);
        ~DescriptorSet() = default;

    private:
        VULKAN_HANDLE(VkDescriptorSet, m_VkDescriptorSet);

    };

}