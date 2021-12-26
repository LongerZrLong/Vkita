#pragma once

#include <memory>
#include <vector>

#include "Core/Base.h"

#include "Vulkan/DescriptorPool.h"
#include "Vulkan/DescriptorSetLayout.h"
#include "Vulkan/DescriptorSet.h"

namespace VKT {

    class DescriptorBinding;

    class DescriptorSetManager
    {
    public:
        NON_COPIABLE(DescriptorSetManager);

        explicit DescriptorSetManager(const std::vector<DescriptorBinding> &descriptorBindings, size_t maxSets = 1);
        ~DescriptorSetManager();

        const Vulkan::DescriptorSetLayout &GetDescriptorSetLayout() const { return *m_DescriptorSetLayout; }
        Vulkan::DescriptorSet &GetDescriptorSet() { return *m_DescriptorSet; }

    private:
        Scope<Vulkan::DescriptorPool> m_DescriptorPool;
        Scope<Vulkan::DescriptorSetLayout> m_DescriptorSetLayout;
        Scope<Vulkan::DescriptorSet> m_DescriptorSet;

    };

}
