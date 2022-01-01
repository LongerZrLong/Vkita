#pragma once

#include "Common.h"
#include "Device.h"

namespace VKT::Vulkan {

    class DeviceMemory
    {
    public:
        DeviceMemory(const DeviceMemory&) = delete;
        DeviceMemory &operator = (const DeviceMemory&) = delete;
        DeviceMemory &operator = (DeviceMemory&&) = delete;

        DeviceMemory(const Device &device, size_t size, uint32_t memoryTypeBits, VkMemoryAllocateFlags allocateFlags, VkMemoryPropertyFlags propertyFlags);
        DeviceMemory(DeviceMemory &&other) noexcept;
        ~DeviceMemory();

        void *Map(size_t offset, size_t size);
        void Unmap();

    private:
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags) const;

    private:
        VULKAN_HANDLE(VkDeviceMemory, m_VkDeviceMemory);

        const Device& m_Device;

    };

}
