#include "PipelineLayout.h"

#include "DescriptorSetLayout.h"
#include "Device.h"

namespace VKT::Vulkan {

    PipelineLayout::PipelineLayout(const Device &device, const DescriptorSetLayout &descriptorSetLayout) :
        m_Device(device)
    {
        VkDescriptorSetLayout descriptorSetLayouts[] = { descriptorSetLayout.GetVkHandle() };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts;
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        Check(vkCreatePipelineLayout(m_Device.GetVkHandle(), &pipelineLayoutInfo, nullptr, &m_VkPipelineLayout),
              "create pipeline layout");
    }

    PipelineLayout::~PipelineLayout()
    {
        if (m_VkPipelineLayout != nullptr)
        {
            vkDestroyPipelineLayout(m_Device.GetVkHandle(), m_VkPipelineLayout, nullptr);
            m_VkPipelineLayout = nullptr;
        }
    }

}
