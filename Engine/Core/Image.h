#pragma once

#include <cstdlib>
#include <string>

namespace VKT {

    class Image
    {
    public:
        explicit Image(const std::string &path);
        Image(const Image &rhs) = delete;
        Image(Image &&rhs) noexcept;

        Image &operator=(const Image &rhs) = delete;
        Image &operator=(Image &&rhs) noexcept;

        ~Image() { delete[] m_Data; }

        uint32_t    m_Width      {0};
        uint32_t    m_Height     {0};
        uint32_t    m_Channel    {0};
        uint8_t    *m_Data       {nullptr};
        size_t      m_DataSize   {0};
        bool        m_IsFloat    {false};

    } __attribute__((aligned(32)));
}