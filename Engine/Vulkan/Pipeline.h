#pragma once

#include "Common.h"
#include "Device.h"

namespace VKT::Vulkan {

    class Pipeline
    {
    public:
        NON_COPIABLE(Pipeline);

        Pipeline(const Device &device, VkGraphicsPipelineCreateInfo *pipelineCreateInfo);
        Pipeline(const Device &device, VkComputePipelineCreateInfo *pipelineCreateInfo);
        ~Pipeline();

    private:
        VULKAN_HANDLE(VkPipeline, m_VkPipeline);

        const Device &m_Device;

    };
}