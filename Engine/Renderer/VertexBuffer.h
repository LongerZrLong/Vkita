#pragma once

#include "Vertex.h"

#include "Core/Base.h"

#include "Vulkan/Buffer.h"
#include "Vulkan/DeviceMemory.h"

namespace VKT {

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

}