#pragma once

#include "Common.h"
#include "Device.h"

namespace VKT::Vulkan {

    class DescriptorPool
    {
    public:
        NON_COPIABLE(DescriptorPool);

        DescriptorPool(const Device &device, VkDescriptorPoolCreateInfo *poolCreateInfo);
        ~DescriptorPool();

    private:
        VULKAN_HANDLE(VkDescriptorPool, m_VkDescriptorPool);

        const Device &m_Device;

    };

}
