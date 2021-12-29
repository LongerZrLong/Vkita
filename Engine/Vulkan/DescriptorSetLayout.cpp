#include "DescriptorSetLayout.h"

namespace VKT::Vulkan {

    DescriptorSetLayout::DescriptorSetLayout(const Device &device, VkDescriptorSetLayoutCreateInfo *layoutInfo)
        : m_Device(device)
    {
        Check(vkCreateDescriptorSetLayout(device.GetVkHandle(), layoutInfo, nullptr, &m_VkDescriptorSetLayout));
    }

    DescriptorSetLayout::~DescriptorSetLayout()
    {
        if (m_VkDescriptorSetLayout != nullptr)
        {
            vkDestroyDescriptorSetLayout(m_Device.GetVkHandle(), m_VkDescriptorSetLayout, nullptr);
            m_VkDescriptorSetLayout = nullptr;
        }
    }

}
