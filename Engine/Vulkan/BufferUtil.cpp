#include "BufferUtil.h"

namespace VKT::Vulkan {

    void BufferUtil::CopyFromStagingBuffer(
        const Device &device,
        const CommandPool &commandPool,
        Buffer &dstBuffer,
        size_t size,
        void *data)
    {
        const auto contentSize = size;

        // Create a temporary host-visible staging buffer.
        auto stagingBuffer = std::make_unique<Buffer>(device, contentSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        auto stagingBufferMemory = stagingBuffer->AllocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        // Copy the host data into the staging buffer.
        const auto ptr = stagingBufferMemory.Map(0, contentSize);
        std::memcpy(ptr, data, contentSize);
        stagingBufferMemory.Unmap();

        // Copy the staging buffer to the device buffer.
        dstBuffer.CopyFrom(commandPool, *stagingBuffer, contentSize);

        // Delete the buffer before the memory
        stagingBuffer.reset();
    }

    void BufferUtil::CreateDeviceBuffer(
        const Device &device,
        const CommandPool &commandPool,
        const char *const name,
        const VkBufferUsageFlags usage,
        size_t size,
        void *data,
        Scope<Buffer> &buffer,
        Scope<DeviceMemory> &memory)
    {
        const auto &debugUtils = device.GetDebugUtils();
        const auto contentSize = size;
        const VkMemoryAllocateFlags allocateFlags = usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
                                                    ? VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT
                                                    : 0;

        buffer = CreateScope<Buffer>(device, contentSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage);
        memory = CreateScope<DeviceMemory>(buffer->AllocateMemory(allocateFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

        debugUtils.SetObjectName(buffer->GetVkHandle(), (name + std::string(" Buffer")).c_str());
        debugUtils.SetObjectName(memory->GetVkHandle(), (name + std::string(" Memory")).c_str());

        CopyFromStagingBuffer(device, commandPool, *buffer, size, data);
    }

}