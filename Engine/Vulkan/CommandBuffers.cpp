#include "CommandBuffers.h"

namespace VKT::Vulkan {

    CommandBuffers::CommandBuffers(const Device &device, const CommandPool &commandPool, uint32_t count)
        : m_Device(device), m_CommandPool(commandPool)
    {
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool.GetVkHandle();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = count;

        m_VkCommandBuffers.resize(count);

        Check(vkAllocateCommandBuffers(m_Device.GetVkHandle(), &allocInfo, m_VkCommandBuffers.data()));
    }

    CommandBuffers::~CommandBuffers()
    {
        if (!m_VkCommandBuffers.empty())
        {
            vkFreeCommandBuffers(m_Device.GetVkHandle(), m_CommandPool.GetVkHandle(), static_cast<uint32_t>(m_VkCommandBuffers.size()), m_VkCommandBuffers.data());
            m_VkCommandBuffers.clear();
        }
    }

    VkCommandBuffer CommandBuffers::Begin(const size_t i) const
    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pInheritanceInfo = nullptr; // Optional

        Check(vkBeginCommandBuffer(m_VkCommandBuffers[i], &beginInfo));

        return m_VkCommandBuffers[i];
    }

    void CommandBuffers::End(const size_t i) const
    {
        Check(vkEndCommandBuffer(m_VkCommandBuffers[i]));
    }

}
