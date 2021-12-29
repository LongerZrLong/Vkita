#pragma once

#include "Common.h"
#include "Device.h"

namespace VKT::Vulkan {

    class PipelineLayout
    {
    public:
        NON_COPIABLE(PipelineLayout);

        PipelineLayout(const Device &device, VkPipelineLayoutCreateInfo *pipelineLayoutInfo);
        ~PipelineLayout();

    private:
        VULKAN_HANDLE(VkPipelineLayout, m_VkPipelineLayout);

        const Device &m_Device;

    };

}
