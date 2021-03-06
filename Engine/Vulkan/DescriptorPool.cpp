#include "DescriptorPool.h"

namespace VKT::Vulkan {

    DescriptorPool::DescriptorPool(const Vulkan::Device &device, VkDescriptorPoolCreateInfo *poolCreateInfo)
        : m_Device(device)
    {
        Check(vkCreateDescriptorPool(device, poolCreateInfo, nullptr, &m_VkDescriptorPool));
    }

    DescriptorPool::~DescriptorPool()
    {
        if (m_VkDescriptorPool != nullptr)
        {
            vkDestroyDescriptorPool(m_Device, m_VkDescriptorPool, nullptr);
            m_VkDescriptorPool = nullptr;
        }
    }

}
