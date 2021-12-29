#include "VulkanBuffer.h"

#include "Core/GraphicsManager.h"

namespace VKT {

    VulkanBuffer::VulkanBuffer(size_t size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags)
        : m_Size(size), m_Descriptor()
    {
        const auto bufferSize = size;

        m_Buffer = CreateScope<Vulkan::Buffer>(g_GraphicsManager->GetDevice(), bufferSize, usageFlags);
        m_DeviceMemory = CreateScope<Vulkan::DeviceMemory>(m_Buffer->AllocateMemory(propertyFlags));

        m_Descriptor.buffer = m_Buffer->GetVkHandle();
        m_Descriptor.offset = 0;
        m_Descriptor.range = size;
    }

    VulkanBuffer::VulkanBuffer(VulkanBuffer &&other) noexcept
        : m_Buffer(other.m_Buffer.release()),
          m_DeviceMemory(other.m_DeviceMemory.release()),
          m_Descriptor(other.m_Descriptor),
          m_Size(other.m_Size)
    {
    }

    void VulkanBuffer::Update(void *data)
    {
        const auto ptr = m_DeviceMemory->Map(0, m_Size);
        std::memcpy(ptr, data, m_Size);
        m_DeviceMemory->Unmap();
    }

    bool VulkanBuffer::Flush(size_t size, size_t offset)
    {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = m_DeviceMemory->GetVkHandle();
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkFlushMappedMemoryRanges(g_GraphicsManager->GetDevice().GetVkHandle(), 1, &mappedRange);
    }

    bool VulkanBuffer::Invalidate(size_t size, size_t offset)
    {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = m_DeviceMemory->GetVkHandle();
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkInvalidateMappedMemoryRanges(g_GraphicsManager->GetDevice().GetVkHandle(), 1, &mappedRange);
    }

}