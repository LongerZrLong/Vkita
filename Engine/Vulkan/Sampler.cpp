#include "Sampler.h"

namespace VKT::Vulkan {

    Sampler::Sampler(const Device &device, VkSamplerCreateInfo *samplerInfo)
        : m_Device(device)
    {
        Check(vkCreateSampler(device, samplerInfo, nullptr, &m_VkSampler));
    }

    Sampler::~Sampler()
    {
        if (m_VkSampler != nullptr)
        {
            vkDestroySampler(m_Device, m_VkSampler, nullptr);
            m_VkSampler = nullptr;
        }
    }

}
