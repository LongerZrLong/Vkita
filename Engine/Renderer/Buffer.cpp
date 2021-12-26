#include "Buffer.h"

#include "Core/GraphicsManager.h"

#include "Vulkan/Buffer.h"
#include "Vulkan/BufferUtil.h"

namespace VKT {

    constexpr auto commonFlags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

    VertexBuffer::VertexBuffer(const std::vector<Vertex> &vertices)
        : m_Count(vertices.size())
    {
        auto flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | commonFlags;
        const Vulkan::CommandPool &commandPool = g_GraphicsManager->GetCommandPool();
        Vulkan::BufferUtil::CreateDeviceBuffer(commandPool, "Vertices", flags, vertices, m_Buffer, m_DeviceMemory);
    }

    VertexBuffer::VertexBuffer(VertexBuffer &&other) noexcept
        : m_Buffer(other.m_Buffer.release()), m_DeviceMemory(other.m_DeviceMemory.release()), m_Count(other.m_Count)
    {
    }

    IndexBuffer::IndexBuffer(const std::vector<uint32_t> &indices)
        : m_Count(indices.size())
    {
        auto flags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | commonFlags;
        const Vulkan::CommandPool &commandPool = g_GraphicsManager->GetCommandPool();
        Vulkan::BufferUtil::CreateDeviceBuffer(commandPool, "indices", flags, indices, m_Buffer, m_DeviceMemory);
    }

    IndexBuffer::IndexBuffer(IndexBuffer &&other) noexcept
        : m_Buffer(other.m_Buffer.release()), m_DeviceMemory(other.m_DeviceMemory.release()), m_Count(other.m_Count)
    {
    }
}