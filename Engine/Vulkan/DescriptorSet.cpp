#include "DescriptorSet.h"

#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "Device.h"

namespace VKT::Vulkan {

    DescriptorSet::DescriptorSet(
        const DescriptorPool &descriptorPool,
        const DescriptorSetLayout &layout,
        const size_t size)
        : m_DescriptorPool(descriptorPool)
    {
        std::vector<VkDescriptorSetLayout> layouts(size, layout.GetVkHandle());

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool.GetVkHandle();
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = layouts.data();

        Check(vkAllocateDescriptorSets(descriptorPool.GetDevice().GetVkHandle(), &allocInfo, &m_VkDescriptorSet),
              "allocate descriptor sets");
    }

    DescriptorSet::~DescriptorSet()
    {
        
    }

}
