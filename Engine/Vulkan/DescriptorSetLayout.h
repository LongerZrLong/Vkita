#pragma once

#include <vector>

#include "Common.h"

namespace VKT {
    class DescriptorBinding;
}

namespace VKT::Vulkan {

    class Device;

    class DescriptorSetLayout
    {
    public:
        NON_COPIABLE(DescriptorSetLayout);

        DescriptorSetLayout(const Device &device, const std::vector<DescriptorBinding> &descriptorBindings);
        ~DescriptorSetLayout();

    private:
        VULKAN_HANDLE(VkDescriptorSetLayout, m_VkDescriptorSetLayout);

        const Device & m_Device;

    };

}
