#pragma once

#include "Common.h"

namespace VKT::Vulkan {

    class DescriptorSetLayout;
    class Device;

    class PipelineLayout
    {
    public:
        NON_COPIABLE(PipelineLayout);

        PipelineLayout(const Device &device, const DescriptorSetLayout &descriptorSetLayout);
        ~PipelineLayout();

    private:
        VULKAN_HANDLE(VkPipelineLayout, m_VkPipelineLayout);

        const Device &m_Device;
    };

}
