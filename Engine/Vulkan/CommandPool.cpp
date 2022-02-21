#include "CommandPool.h"

namespace VKT::Vulkan {

    CommandPool::CommandPool(const Device &device, const uint32_t queueFamilyIndex, const bool allowReset)
        : m_Device(device)
    {
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndex;
        poolInfo.flags = allowReset ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 0;

        Check(vkCreateCommandPool(device, &poolInfo, nullptr, &m_VkCommandPool));
    }

    CommandPool::~CommandPool()
    {
        if (m_VkCommandPool != nullptr)
        {
            vkDestroyCommandPool(m_Device, m_VkCommandPool, nullptr);
            m_VkCommandPool = nullptr;
        }
    }

}
