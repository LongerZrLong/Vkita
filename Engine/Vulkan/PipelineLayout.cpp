#include "PipelineLayout.h"

namespace VKT::Vulkan {

    PipelineLayout::PipelineLayout(const Device &device, VkPipelineLayoutCreateInfo *pipelineLayoutInfo)
        : m_Device(device)
    {
        Check(vkCreatePipelineLayout(m_Device, pipelineLayoutInfo, nullptr, &m_VkPipelineLayout));
    }

    PipelineLayout::~PipelineLayout()
    {
        if (m_VkPipelineLayout != nullptr)
        {
            vkDestroyPipelineLayout(m_Device, m_VkPipelineLayout, nullptr);
            m_VkPipelineLayout = nullptr;
        }
    }

}
