#pragma once

#include "Common.h"

namespace VKT::Vulkan {

    class Device;

    class DeviceMemory
    {
    public:
        DeviceMemory(const DeviceMemory&) = delete;
        DeviceMemory &operator = (const DeviceMemory&) = delete;
        DeviceMemory &operator = (DeviceMemory&&) = delete;

        DeviceMemory(const Device &device, size_t size, uint32_t memoryTypeBits, VkMemoryAllocateFlags allocateFlags, VkMemoryPropertyFlags propertyFlags);
        DeviceMemory(DeviceMemory &&other) noexcept;
        ~DeviceMemory();

        const Device &GetDevice() const { return m_Device; }

        void *Map(size_t offset, size_t size);
        void Unmap();

    private:
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags) const;

    private:
        VULKAN_HANDLE(VkDeviceMemory, m_VkDeviceMemory);

        const Device& m_Device;

    };

}
