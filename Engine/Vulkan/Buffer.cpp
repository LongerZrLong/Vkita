#include "Buffer.h"

#include "SingleTimeCommands.h"

namespace VKT::Vulkan {

    Buffer::Buffer(const Device &device, const size_t size, const VkBufferUsageFlags usageFlags)
        : m_Device(device)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usageFlags;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        Check(vkCreateBuffer(device, &bufferInfo, nullptr, &m_VkBuffer));
    }

    Buffer::~Buffer()
    {
        if (m_VkBuffer != nullptr)
        {
            vkDestroyBuffer(m_Device, m_VkBuffer, nullptr);
            m_VkBuffer = nullptr;
        }
    }

    DeviceMemory Buffer::AllocateMemory(const VkMemoryPropertyFlags propertyFlags)
    {
        return AllocateMemory(0, propertyFlags);
    }

    DeviceMemory Buffer::AllocateMemory(const VkMemoryAllocateFlags allocateFlags, const VkMemoryPropertyFlags propertyFlags)
    {
        const auto requirements = GetVkMemoryRequirements();
        DeviceMemory memory(m_Device, requirements.size, requirements.memoryTypeBits, allocateFlags, propertyFlags);

        Check(vkBindBufferMemory(m_Device, m_VkBuffer, memory, 0));

        return memory;
    }

    VkMemoryRequirements Buffer::GetVkMemoryRequirements() const
    {
        VkMemoryRequirements requirements;
        vkGetBufferMemoryRequirements(m_Device, m_VkBuffer, &requirements);
        return requirements;
    }

    VkDeviceAddress Buffer::GetVkDeviceAddress() const
    {
        VkBufferDeviceAddressInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        info.pNext = nullptr;
        info.buffer = *this;

        return vkGetBufferDeviceAddress(m_Device, &info);
    }

    void Buffer::CopyFrom(const CommandPool &commandPool, const Buffer &src, VkDeviceSize size) const
    {
        SingleTimeCommands::Submit(m_Device, commandPool, [&](VkCommandBuffer commandBuffer)
        {
            VkBufferCopy copyRegion = {};
            copyRegion.srcOffset = 0; // Optional
            copyRegion.dstOffset = 0; // Optional
            copyRegion.size = size;

            vkCmdCopyBuffer(commandBuffer, src, *this, 1, &copyRegion);
        });
    }

}
