#pragma once

#include <vector>

#include "Common.h"

namespace VKT {
    class DescriptorBinding;
}

namespace VKT::Vulkan {

    class Device;

    class DescriptorPool
    {
    public:
        NON_COPIABLE(DescriptorPool);

        DescriptorPool(const Device &device, const std::vector<DescriptorBinding> &descriptorBindings, size_t maxSets);
        ~DescriptorPool();

        const Device &GetDevice() const { return m_Device; }

    private:
        VULKAN_HANDLE(VkDescriptorPool, m_VkDescriptorPool);

        const Device &m_Device;

    };

}
