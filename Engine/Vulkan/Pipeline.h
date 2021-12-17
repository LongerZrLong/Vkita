#pragma once

#include "Common.h"

namespace VKT::Vulkan {

    class Device;

    class Pipeline
    {
    public:
        NON_COPIABLE(Pipeline);

        Pipeline(const Device &device, VkGraphicsPipelineCreateInfo pipelineCreateInfo);
        ~Pipeline();

    private:
        VULKAN_HANDLE(VkPipeline, m_VkPipeline);

        const Device &m_Device;

    };
}