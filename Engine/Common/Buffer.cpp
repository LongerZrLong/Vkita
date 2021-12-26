#include "Buffer.h"

#include <cstring>

namespace VKT {

    Buffer::Buffer()
        : m_Data(nullptr), m_Size(0), m_Alignment(alignof(uint32_t))
    {
    }

    Buffer::Buffer(size_t size, size_t alignment)
        : m_Size(size), m_Alignment(alignment)
    {
        m_Data = reinterpret_cast<uint8_t*>(g_MemoryManager->Allocate(size, alignment));
    }

    Buffer::Buffer(const Buffer &rhs)
        : m_Size(rhs.m_Size), m_Alignment(rhs.m_Alignment)
    {
        m_Data = reinterpret_cast<uint8_t*>(g_MemoryManager->Allocate(rhs.m_Size, rhs.m_Alignment));
        memcpy(m_Data, rhs.m_Data, rhs.m_Size);
    }

    Buffer::Buffer(Buffer &&rhs) noexcept
        : m_Data(rhs.m_Data), m_Size(rhs.m_Size), m_Alignment(rhs.m_Alignment)
    {
        rhs.m_Data = nullptr;
        rhs.m_Size = 0;
        rhs.m_Alignment = 4;
    }

    Buffer &Buffer::operator=(const Buffer &rhs)
    {
        if (m_Size >= rhs.m_Size && m_Alignment == rhs.m_Alignment)
        {
            memcpy(m_Data, rhs.m_Data, rhs.m_Size);
        }
        else
        {
            if (m_Data)
                g_MemoryManager->Free(m_Data, m_Size);

            m_Data = reinterpret_cast<uint8_t*>(g_MemoryManager->Allocate(rhs.m_Size, rhs.m_Alignment));
            memcpy(m_Data, rhs.m_Data, rhs.m_Size);
            m_Size =  rhs.m_Size;
            m_Alignment = rhs.m_Alignment;
        }

        return *this;
    }

    Buffer &Buffer::operator=(Buffer &&rhs) noexcept
    {
        if (m_Data)
            g_MemoryManager->Free(m_Data, m_Size);

        m_Data = rhs.m_Data;
        m_Size = rhs.m_Size;
        m_Alignment = rhs.m_Alignment;

        rhs.m_Data = nullptr;
        rhs.m_Size = 0;
        rhs.m_Alignment = 4;

        return *this;
    }

    Buffer::~Buffer()
    {
        if (m_Data)
            g_MemoryManager->Free(m_Data, m_Size);

        m_Data = nullptr;
    }

    uint8_t *Buffer::MoveData()
    {
        uint8_t* tmp = m_Data;
        m_Data = nullptr;
        m_Size = 0;
        return tmp;
    }

    void Buffer::SetData(uint8_t *data, size_t size)
    {
        if (m_Data != nullptr)
        {
            delete[] m_Data;
        }
        m_Data = data;
        m_Size = size;
    }

}