#pragma once

#include <memory>

#include "Common/Base.h"

namespace VKT {

    namespace Vulkan {
        class CommandPool;
        class DeviceMemory;
        class Image;
        class ImageView;
        class Sampler;
    }

    class Texture2D
    {
    public:
        NON_COPIABLE(Texture2D);

        Texture2D(const std::string &path);
        Texture2D(uint32_t width, uint32_t height);
        ~Texture2D();

        void SetData(void *data, uint32_t size);

        const Vulkan::ImageView &GetImageView() const { return *m_ImageView; }
        const Vulkan::Sampler &GetSampler() const { return *m_Sampler; }

    private:
        void PrepareTexture2D();

    private:
        Scope<Vulkan::Image> m_Image;
        Scope<Vulkan::DeviceMemory> m_DeviceMemory;
        Scope<Vulkan::ImageView> m_ImageView;
        Scope<Vulkan::Sampler> m_Sampler;

        uint32_t m_Width, m_Height;

    };

}