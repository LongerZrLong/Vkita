#include "Image.h"

#include <stb_image.h>

namespace VKT {

    Image::Image(const std::string &path)
    {
        int imgWidth, imgHeight, imgChannels;
        stbi_uc* pixels = stbi_load(path.c_str(), &imgWidth, &imgHeight, &imgChannels, STBI_rgb_alpha);

        m_DataSize = imgWidth * imgHeight * 4;

        if (!pixels)
        {
            throw std::runtime_error("failed to load texture image!");
        }

        m_Data = pixels;

        m_Width = imgWidth;
        m_Height = imgHeight;
        m_Channel = 4;
        m_IsFloat = false;
    }

    Image::Image(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height), m_Channel(4), m_DataSize(width * height * 4), m_IsFloat(false),
          m_Data(nullptr)
    {
    }

    Image::Image(Image &&rhs) noexcept
        : m_Width(rhs.m_Width), m_Height(rhs.m_Height), m_Channel(rhs.m_Channel), m_Data(rhs.m_Data), m_DataSize(rhs.m_DataSize), m_IsFloat(rhs.m_IsFloat)
    {
        rhs.m_Width = 0;
        rhs.m_Height = 0;
        rhs.m_Channel = 0;
        rhs.m_Data = nullptr;
        rhs.m_DataSize = 0;
        rhs.m_IsFloat = false;
    }

    Image &Image::operator=(Image &&rhs) noexcept
    {
        if (this != &rhs)
        {
            m_Width = rhs.m_Width;
            m_Height = rhs.m_Height;
            m_Channel = rhs.m_Channel;
            m_Data = rhs.m_Data;
            m_DataSize = rhs.m_DataSize;
            m_IsFloat = rhs.m_IsFloat;

            rhs.m_Width = 0;
            rhs.m_Height = 0;
            rhs.m_Channel = 0;
            rhs.m_Data = nullptr;
            rhs.m_DataSize = 0;
            rhs.m_IsFloat = false;
        }
        return *this;
    }
}