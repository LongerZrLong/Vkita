#include "Sampler.h"

#include "Device.h"

namespace VKT::Vulkan {

    Sampler::Sampler(const Device &device, const SamplerConfig &config)
        : m_Device(device)
    {
        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = config.MagFilter;
        samplerInfo.minFilter = config.MinFilter;
        samplerInfo.addressModeU = config.AddressModeU;
        samplerInfo.addressModeV = config.AddressModeV;
        samplerInfo.addressModeW = config.AddressModeW;
        samplerInfo.anisotropyEnable = config.AnisotropyEnable;
        samplerInfo.maxAnisotropy = config.MaxAnisotropy;
        samplerInfo.borderColor = config.BorderColor;
        samplerInfo.unnormalizedCoordinates = config.UnnormalizedCoordinates;
        samplerInfo.compareEnable = config.CompareEnable;
        samplerInfo.compareOp = config.CompareOp;
        samplerInfo.mipmapMode = config.MipmapMode;
        samplerInfo.mipLodBias = config.MipLodBias;
        samplerInfo.minLod = config.MinLod;
        samplerInfo.maxLod = config.MaxLod;

        if (vkCreateSampler(device.GetVkHandle(), &samplerInfo, nullptr, &m_VkSampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create sampler");
        }
    }

    Sampler::~Sampler()
    {
        if (m_VkSampler != nullptr)
        {
            vkDestroySampler(m_Device.GetVkHandle(), m_VkSampler, nullptr);
            m_VkSampler = nullptr;
        }
    }

}
