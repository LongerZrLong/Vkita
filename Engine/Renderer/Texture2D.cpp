#include "Texture2D.h"

#include <stb_image.h>

#include "Core/GraphicsManager.h"

#include "Vulkan/Buffer.h"
#include "Vulkan/CommandPool.h"
#include "Vulkan/ImageView.h"
#include "Vulkan/Image.h"
#include "Vulkan/DeviceMemory.h"
#include "Vulkan/Sampler.h"

namespace VKT {

    Texture2D::Texture2D(const Image &img)
        : m_Width(img.m_Width), m_Height(img.m_Height)
    {
        PrepareTexture2D();
        SetData(img.m_Data, img.m_DataSize);
    }

    Texture2D::Texture2D(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height)
    {
        PrepareTexture2D();
    }

    void Texture2D::SetData(void *data, uint32_t size)
    {
        const Vulkan::Device &device = g_GraphicsManager->GetDevice();
        const Vulkan::CommandPool &commandPool = g_GraphicsManager->GetCommandPool();

        // Create a host staging buffer and copy the image into it.
        VkDeviceSize imageSize = size;
        auto stagingBuffer = CreateScope<Vulkan::Buffer>(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        auto stagingBufferMemory = stagingBuffer->AllocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        void *ptr = stagingBufferMemory.Map(0, imageSize);
        std::memcpy(ptr, data, imageSize);
        stagingBufferMemory.Unmap();

        // Transfer the data to device side.
        m_Image->TransitionImageLayout(device, commandPool, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        m_Image->CopyFrom(device, commandPool, *stagingBuffer);
        m_Image->TransitionImageLayout(device, commandPool, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

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

        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_NEAREST;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.anisotropyEnable = true;
        samplerInfo.maxAnisotropy = 16;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = false;
        samplerInfo.compareEnable = false;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        m_Sampler = CreateScope<Vulkan::Sampler>(device, &samplerInfo);
    }

    Texture2D::~Texture2D()
    {
        m_Sampler.reset();
        m_ImageView.reset();
        m_Image.reset();
        m_DeviceMemory.reset();
    }

}
