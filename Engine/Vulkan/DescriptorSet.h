#pragma once

#include <map>
#include <vector>

#include "Common.h"

namespace VKT::Vulkan {

    class Buffer;
    class DescriptorPool;
    class DescriptorSetLayout;
    class ImageView;

    class DescriptorSet
    {
    public:
        NON_COPIABLE(DescriptorSet);

        DescriptorSet(const DescriptorPool &descriptorPool, const DescriptorSetLayout &layout, size_t size);
        ~DescriptorSet();

    private:
        VULKAN_HANDLE(VkDescriptorSet, m_VkDescriptorSet);

        const DescriptorPool &m_DescriptorPool;

    };

}