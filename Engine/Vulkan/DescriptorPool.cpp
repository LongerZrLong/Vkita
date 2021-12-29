#include "DescriptorPool.h"

#include "Device.h"

#include "Renderer/DescriptorBinding.h"

namespace VKT::Vulkan {

    DescriptorPool::DescriptorPool(const Vulkan::Device &device, const std::vector<DescriptorBinding> &descriptorBindings, const size_t maxSets)
        : m_Device(device)
    {
        std::vector<VkDescriptorPoolSize> poolSizes;

        for (const auto &binding : descriptorBindings)
        {
            poolSizes.push_back(VkDescriptorPoolSize{ binding.Type, static_cast<uint32_t>(binding.DescriptorCount*maxSets )});
        }

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(maxSets);

        Check(vkCreateDescriptorPool(device.GetVkHandle(), &poolInfo, nullptr, &m_VkDescriptorPool));
    }

    DescriptorPool::~DescriptorPool()
    {
        if (m_VkDescriptorPool != nullptr)
        {
            vkDestroyDescriptorPool(m_Device.GetVkHandle(), m_VkDescriptorPool, nullptr);
            m_VkDescriptorPool = nullptr;
        }
    }

}
