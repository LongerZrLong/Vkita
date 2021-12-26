#pragma once

#include <cstdlib>

#include "MemoryManager.h"

namespace VKT {

    class Buffer
    {
    public:
        Buffer();
        Buffer(size_t size, size_t alignment = 4);
        Buffer(const Buffer &rhs);
        Buffer(Buffer &&rhs) noexcept ;

        Buffer &operator=(const Buffer &rhs);
        Buffer &operator=(Buffer &&rhs) noexcept ;

        ~Buffer();

        uint8_t *GetData() { return m_Data; }
        const uint8_t *GetData() const { return m_Data; }
        size_t GetDataSize() const { return m_Size; }

        uint8_t* MoveData();
        void SetData(uint8_t* data, size_t size);

    protected:
        uint8_t *m_Data;
        size_t m_Size;
        size_t m_Alignment;

    };
}