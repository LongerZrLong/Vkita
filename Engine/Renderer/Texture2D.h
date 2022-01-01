#pragma once

#include <memory>

#include <vulkan/vulkan.h>

#include "Core/Base.h"
#include "Core/Image.h"

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

        explicit Texture2D(const Image &img);
        Texture2D(uint32_t width, uint32_t height);
        ~Texture2D();

        void SetData(void *data, uint32_t size);

        VkDescriptorImageInfo &GetDescriptor() { return m_Descriptor; }

    private:
        void PrepareTexture2D();

    private:
        Scope<Vulkan::Image> m_Image;
        Scope<Vulkan::DeviceMemory> m_DeviceMemory;
        Scope<Vulkan::ImageView> m_ImageView;
        Scope<Vulkan::Sampler> m_Sampler;

        VkDescriptorImageInfo m_Descriptor;

        uint32_t m_Width, m_Height;

    };

}