#include "Pipeline.h"

#include "Device.h"

namespace VKT::Vulkan {

    Pipeline::Pipeline(const Device &device, VkGraphicsPipelineCreateInfo pipelineCreateInfo)
        : m_Device(device)
    {
        Check(vkCreateGraphicsPipelines(device.GetVkHandle(), nullptr, 1, &pipelineCreateInfo, nullptr, &m_VkPipeline));
    }

    Pipeline::~Pipeline()
    {
        if (m_VkPipeline != nullptr)
        {
            vkDestroyPipeline(m_Device.GetVkHandle(), m_VkPipeline, nullptr);
            m_VkPipeline = nullptr;
        }
    }
}