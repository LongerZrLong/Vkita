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

        void Update(VkDescriptorType type, uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
        void Update(VkDescriptorType type, uint32_t binding, VkDescriptorImageInfo *imageInfo);

    private:
        VULKAN_HANDLE(VkDescriptorSet, m_VkDescriptorSet);

        const Device &m_Device;
    };

}