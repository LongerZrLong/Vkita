#pragma once

#include "Core/Base.h"

#include "Vulkan/Buffer.h"
#include "Vulkan/DeviceMemory.h"

namespace VKT::Rendering {
    
    class Buffer
    {
    public:
        Buffer(const Buffer&) = delete;
        Buffer &operator = (const Buffer&) = delete;
        Buffer &operator = (Buffer&&) = delete;

        Buffer(size_t size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags);
        Buffer(Buffer &&other) noexcept;
        ~Buffer() = default;

        void Update(void *data);

        bool Flush(size_t size, size_t offset);
        bool Invalidate(size_t size, size_t offset);

        const VKT::Vulkan::Buffer &GetBuffer() const { return *m_Buffer; }
        const VKT::Vulkan::DeviceMemory &GetDeviceMemory() const { return *m_DeviceMemory; }
        const VkDescriptorBufferInfo &GetDescriptor() const { return m_Descriptor; }

        size_t GetSize() const { return m_Size; }

    private:
        Scope<VKT::Vulkan::Buffer> m_Buffer;
        Scope<VKT::Vulkan::DeviceMemory> m_DeviceMemory;

        size_t m_Size;

        VkDescriptorBufferInfo m_Descriptor;
    };
    
}