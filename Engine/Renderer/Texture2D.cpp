#include "Texture2D.h"

#include <string>

#include <stb_image.h>

#include "Core/FileSystem.h"
#include "Core/GraphicsManager.h"

#include "Vulkan/Buffer.h"
#include "Vulkan/CommandPool.h"
#include "Vulkan/ImageView.h"
#include "Vulkan/Image.h"
#include "Vulkan/DeviceMemory.h"
#include "Vulkan/Sampler.h"

namespace VKT {

    Texture2D::Texture2D(const std::string &path)
    {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels)
        {
            throw std::runtime_error("failed to load texture image!");
        }

        m_Width = texWidth;
        m_Height = texHeight;

        PrepareTexture2D();

        SetData(pixels, imageSize);
    }

    Texture2D::Texture2D(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height)
    {
        const Vulkan::Device &device = g_GraphicsManager->GetDevice();

        // Create the device side image, memory, view and sampler.
        m_Image = CreateScope<Vulkan::Image>(device, VkExtent2D{ width, width }, VK_FORMAT_R8G8B8A8_UNORM);
        m_DeviceMemory = CreateScope<Vulkan::DeviceMemory>(m_Image->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
        m_ImageView = CreateScope<Vulkan::ImageView>(device, m_Image->GetVkHandle(), m_Image->GetVkFormat(), VK_IMAGE_ASPECT_COLOR_BIT);
        m_Sampler = CreateScope<Vulkan::Sampler>(device, Vulkan::SamplerConfig());
    }

    void Texture2D::SetData(void *data, uint32_t size)
    {
        const Vulkan::CommandPool &commandPool = g_GraphicsManager->GetCommandPool();
        const Vulkan::Device &device = g_GraphicsManager->GetDevice();

        // Create a host staging buffer and copy the image into it.
        VkDeviceSize imageSize = size;
        auto stagingBuffer = CreateScope<Vulkan::Buffer>(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        auto stagingBufferMemory = stagingBuffer->AllocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        void *ptr = stagingBufferMemory.Map(0, imageSize);
        std::memcpy(ptr, data, imageSize);
        stagingBufferMemory.Unmap();

        // Transfer the data to device side.
        m_Image->TransitionImageLayout(commandPool, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        m_Image->CopyFrom(commandPool, *stagingBuffer);
        m_Image->TransitionImageLayout(commandPool, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        // Delete the buffer before the memory
        stagingBuffer.reset();
    }

    void Texture2D::PrepareTexture2D()
    {
        const Vulkan::Device &device = g_GraphicsManager->GetDevice();

        // Create the device side image, memory, view and sampler.
        m_Image = CreateScope<Vulkan::Image>(device, VkExtent2D{ m_Width, m_Height }, VK_FORMAT_R8G8B8A8_UNORM);
        m_DeviceMemory = CreateScope<Vulkan::DeviceMemory>(m_Image->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
        m_ImageView = CreateScope<Vulkan::ImageView>(device, m_Image->GetVkHandle(), m_Image->GetVkFormat(), VK_IMAGE_ASPECT_COLOR_BIT);
        m_Sampler = CreateScope<Vulkan::Sampler>(device, Vulkan::SamplerConfig());
    }

    Texture2D::~Texture2D()
    {
        m_Sampler.reset();
        m_ImageView.reset();
        m_Image.reset();
        m_DeviceMemory.reset();
    }

}
