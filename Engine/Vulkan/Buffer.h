#pragma once

#include "Common.h"

namespace VKT::Vulkan {

    class Device;
    class CommandPool;
    class DeviceMemory;

    class Buffer
    {
    public:
        NON_COPIABLE(Buffer);

        Buffer(const Device &device, size_t size, VkBufferUsageFlags usageFlags);
        ~Buffer();

        const Device &GetDevice() const { return m_Device; }

        DeviceMemory AllocateMemory(VkMemoryPropertyFlags propertyFlags);
        DeviceMemory AllocateMemory(VkMemoryAllocateFlags allocateFlags, VkMemoryPropertyFlags propertyFlags);

        VkMemoryRequirements GetVkMemoryRequirements() const;
        VkDeviceAddress GetVkDeviceAddress() const;

        void CopyFrom(const CommandPool &commandPool, const Buffer &src, VkDeviceSize size);

    private:
        VULKAN_HANDLE(VkBuffer, m_VkBuffer);

        const Device &m_Device;
    };
}
