#include "DescriptorSet.h"

#include "Initializers.h"

namespace VKT::Vulkan {

    DescriptorSet::DescriptorSet(const Device &device, VkDescriptorSetAllocateInfo *allocInfo)
        : m_Device(device)
    {
        Check(vkAllocateDescriptorSets(device.GetVkHandle(), allocInfo, &m_VkDescriptorSet));
    }

    void DescriptorSet::Update(VkDescriptorType type, uint32_t binding, VkDescriptorBufferInfo *bufferInfo)
    {
        VkWriteDescriptorSet write = Vulkan::Initializers::writeDescriptorSet(m_VkDescriptorSet, type, binding, bufferInfo);
        vkUpdateDescriptorSets(m_Device.GetVkHandle(), 1, &write, 0, nullptr);
    }

    void DescriptorSet::Update(VkDescriptorType type, uint32_t binding, VkDescriptorImageInfo *imageInfo)
    {
        VkWriteDescriptorSet write = Vulkan::Initializers::writeDescriptorSet(m_VkDescriptorSet, type, binding, imageInfo);
        vkUpdateDescriptorSets(m_Device.GetVkHandle(), 1, &write, 0, nullptr);
    }
}
