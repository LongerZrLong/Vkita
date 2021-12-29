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
        static void CopyFromStagingBuffer(const CommandPool &commandPool, Buffer &dstBuffer, size_t size, void *data);

        static void CreateDeviceBuffer(
            const CommandPool &commandPool,
            const char *name,
            VkBufferUsageFlags usage,
            size_t size,
            void *data,
            Scope<Buffer> &buffer,
            Scope<DeviceMemory> &memory);
    };

}
