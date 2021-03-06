#include "VertexBuffer.h"

#include "Core/GraphicsManager.h"

#include "Vulkan/BufferUtil.h"

namespace VKT::Rendering {

    VertexBuffer::VertexBuffer(const std::vector<Vertex> &vertices)
        : m_Count(vertices.size())
    {
        auto flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        const VKT::Vulkan::Device &device = g_GraphicsManager->GetContext().GetDevice();
        const VKT::Vulkan::CommandPool &commandPool = g_GraphicsManager->GetContext().GetCommandPool();
        size_t size = sizeof(vertices[0]) * m_Count;
        VKT::Vulkan::BufferUtil::CreateDeviceBuffer(device, commandPool, "Vertices", flags, size, (void*)vertices.data(), m_Buffer, m_DeviceMemory);
    }

    VertexBuffer::VertexBuffer(VertexBuffer &&other) noexcept
        : m_Buffer(other.m_Buffer.release()), m_DeviceMemory(other.m_DeviceMemory.release()), m_Count(other.m_Count)
    {
    }
}