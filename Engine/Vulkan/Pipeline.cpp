#include "Pipeline.h"

namespace VKT::Vulkan {

    Pipeline::Pipeline(const Device &device, VkGraphicsPipelineCreateInfo *pipelineCreateInfo)
        : m_Device(device)
    {
        Check(vkCreateGraphicsPipelines(device, nullptr, 1, pipelineCreateInfo, nullptr, &m_VkPipeline));
    }

    Pipeline::Pipeline(const Device &device, VkComputePipelineCreateInfo *pipelineCreateInfo)
        : m_Device(device)
    {
        Check(vkCreateComputePipelines(device, nullptr, 1, pipelineCreateInfo, nullptr, &m_VkPipeline));
    }

    Pipeline::~Pipeline()
    {
        if (m_VkPipeline != nullptr)
        {
            vkDestroyPipeline(m_Device, m_VkPipeline, nullptr);
            m_VkPipeline = nullptr;
        }
    }
}