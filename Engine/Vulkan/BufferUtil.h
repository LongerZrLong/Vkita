#pragma once

#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "Buffer.h"
#include "CommandPool.h"
#include "Device.h"
#include "DeviceMemory.h"

namespace VKT::Vulkan {
    
    class BufferUtil
    {
    public:
        template <class T>
        static void CopyFromStagingBuffer(const CommandPool &commandPool, Buffer &dstBuffer, const std::vector<T> &content);

        template <class T>
        static void CreateDeviceBuffer(
            const CommandPool &commandPool,
            const char *name,
            VkBufferUsageFlags usage,
            const std::vector<T> &content,
            Scope<Buffer> &buffer,
            Scope<DeviceMemory> &memory);
    };

    template <class T>
    void BufferUtil::CopyFromStagingBuffer(const CommandPool &commandPool, Buffer &dstBuffer, const std::vector<T> &content)
    {
        const auto &device = commandPool.GetDevice();
        const auto contentSize = sizeof(content[0]) * content.size();

        // Create a temporary host-visible staging buffer.
        auto stagingBuffer = std::make_unique<Buffer>(device, contentSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        auto stagingBufferMemory = stagingBuffer->AllocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        // Copy the host data into the staging buffer.
        const auto data = stagingBufferMemory.Map(0, contentSize);
        std::memcpy(data, content.data(), contentSize);
        stagingBufferMemory.Unmap();

        // Copy the staging buffer to the device buffer.
        dstBuffer.CopyFrom(commandPool, *stagingBuffer, contentSize);

        // Delete the buffer before the memory
        stagingBuffer.reset();
    }

    template <class T>
    void BufferUtil::CreateDeviceBuffer(
        const CommandPool &commandPool,
        const char *const name,
        const VkBufferUsageFlags usage,
        const std::vector<T> &content,
        Scope<Buffer> &buffer,
        Scope<DeviceMemory> &memory)
    {
        const auto &device = commandPool.GetDevice();
        const auto &debugUtils = device.GetDebugUtils();
        const auto contentSize = sizeof(content[0]) * content.size();
        const VkMemoryAllocateFlags allocateFlags = usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
                                                    ? VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT
                                                    : 0;

        buffer = CreateScope<Buffer>(device, contentSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage);
        memory = CreateScope<DeviceMemory>(buffer->AllocateMemory(allocateFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

        debugUtils.SetObjectName(buffer->GetVkHandle(), (name + std::string(" Buffer")).c_str());
        debugUtils.SetObjectName(memory->GetVkHandle(), (name + std::string(" Memory")).c_str());

        CopyFromStagingBuffer(commandPool, *buffer, content);
    }
}
