#include "DescriptorSet.h"

namespace VKT::Vulkan {

    DescriptorSet::DescriptorSet(const Device &device, VkDescriptorSetAllocateInfo *allocInfo)
    {
        Check(vkAllocateDescriptorSets(device.GetVkHandle(), allocInfo, &m_VkDescriptorSet));
    }

}
