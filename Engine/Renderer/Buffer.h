#pragma once

#include "Vertex.h"

#include "Common/Base.h"

namespace VKT {

    namespace Vulkan {
        class Buffer;
        class DeviceMemory;
    }

    class VertexBuffer
    {
    public:
        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer &operator = (const VertexBuffer&) = delete;
        VertexBuffer &operator = (VertexBuffer&&) = delete;
        
        explicit VertexBuffer(const std::vector<Vertex> &vertices);
        VertexBuffer(VertexBuffer &&other) noexcept;
        ~VertexBuffer() = default;

        const Vulkan::Buffer &GetBuffer() const { return *m_Buffer; }

    private:
        Scope<Vulkan::Buffer> m_Buffer;
        Scope<Vulkan::DeviceMemory> m_DeviceMemory;

        uint32_t m_Count;

    };

    class IndexBuffer
    {
    public:
        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer &operator = (const IndexBuffer&) = delete;
        IndexBuffer &operator = (IndexBuffer&&) = delete;
        
        explicit IndexBuffer(const std::vector<uint32_t> &indices);
        IndexBuffer(IndexBuffer &&other) noexcept;
        ~IndexBuffer() = default;

        const Vulkan::Buffer &GetBuffer() const { return *m_Buffer; }
        uint32_t GetCount() const { return m_Count; }

    private:
        Scope<Vulkan::Buffer> m_Buffer;
        Scope<Vulkan::DeviceMemory> m_DeviceMemory;

        uint32_t m_Count;

    };
}