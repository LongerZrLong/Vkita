#pragma once

#include "Core/Base.h"

#include "Vulkan/Buffer.h"
#include "Vulkan/DeviceMemory.h"

namespace VKT {
    
    class VulkanBuffer
    {
    public:
        VulkanBuffer(const VulkanBuffer&) = delete;
        VulkanBuffer &operator = (const VulkanBuffer&) = delete;
        VulkanBuffer &operator = (VulkanBuffer&&) = delete;

        VulkanBuffer(size_t size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags);
        VulkanBuffer(VulkanBuffer &&other) noexcept;
        ~VulkanBuffer() = default;

        void Update(void *data);

        bool Flush(size_t size, size_t offset);
        bool Invalidate(size_t size, size_t offset);

        const Vulkan::Buffer &GetBuffer() const { return *m_Buffer; }
        const VkDescriptorBufferInfo &GetDescriptor() const { return m_Descriptor; }

    private:
        Scope<Vulkan::Buffer> m_Buffer;
        Scope<Vulkan::DeviceMemory> m_DeviceMemory;

        size_t m_Size;

        VkDescriptorBufferInfo m_Descriptor;
    };
    
}