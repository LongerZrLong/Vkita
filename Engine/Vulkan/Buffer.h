#pragma once

#include "Common.h"
#include "Device.h"
#include "DeviceMemory.h"
#include "CommandPool.h"

namespace VKT::Vulkan {

    class Buffer
    {
    public:
        NON_COPIABLE(Buffer);

        Buffer(const Device &device, size_t size, VkBufferUsageFlags usageFlags);
        ~Buffer();

        DeviceMemory AllocateMemory(VkMemoryPropertyFlags propertyFlags);
        DeviceMemory AllocateMemory(VkMemoryAllocateFlags allocateFlags, VkMemoryPropertyFlags propertyFlags);

        VkMemoryRequirements GetVkMemoryRequirements() const;
        VkDeviceAddress GetVkDeviceAddress() const;

        void CopyFrom(const CommandPool &commandPool, const Buffer &src, VkDeviceSize size) const;

    private:
        VULKAN_HANDLE(VkBuffer, m_VkBuffer);

        const Device &m_Device;
    };
}
