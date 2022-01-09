#include "IndexBuffer.h"

#include <vector>

#include "Core/GraphicsManager.h"

#include "Vulkan/BufferUtil.h"

namespace VKT::Rendering {

    IndexBuffer::IndexBuffer(const std::vector<uint32_t> &indices)
        : m_Count(indices.size())
    {
        auto flags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        const VKT::Vulkan::Device &device = g_GraphicsManager->GetContext().GetDevice();
        const VKT::Vulkan::CommandPool &commandPool = g_GraphicsManager->GetContext().GetCommandPool();
        size_t size = sizeof(indices[0]) * m_Count;
        VKT::Vulkan::BufferUtil::CreateDeviceBuffer(device, commandPool, "Indices", flags, size, (void*)indices.data(), m_Buffer, m_DeviceMemory);
    }

    IndexBuffer::IndexBuffer(IndexBuffer &&other) noexcept
        : m_Buffer(other.m_Buffer.release()), m_DeviceMemory(other.m_DeviceMemory.release()), m_Count(other.m_Count)
    {
    }

}