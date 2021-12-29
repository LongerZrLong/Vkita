#pragma once

#include <cstdlib>

#include "Core/Base.h"

#include "Vulkan/Buffer.h"
#include "Vulkan/DeviceMemory.h"

namespace VKT {

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