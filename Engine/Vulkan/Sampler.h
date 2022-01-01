#pragma once

#include "Common.h"
#include "Device.h"

namespace VKT::Vulkan {

    class Sampler
    {
    public:
        NON_COPIABLE(Sampler);

        Sampler(const Device &device, VkSamplerCreateInfo *samplerInfo);
        ~Sampler();

    private:
        VULKAN_HANDLE(VkSampler, m_VkSampler);

        const Device &m_Device;

    };

}
