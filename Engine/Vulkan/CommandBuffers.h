#pragma once

#include <vector>

#include "Common.h"
#include "Device.h"
#include "CommandPool.h"

namespace VKT::Vulkan {

    class CommandBuffers
    {
    public:
        NON_COPIABLE(CommandBuffers);

        CommandBuffers(const Device &device, const CommandPool &commandPool, uint32_t count);
        ~CommandBuffers();

        uint32_t GetSize() const { return static_cast<uint32_t>(m_VkCommandBuffers.size()); }

        const VkCommandBuffer &operator [] (const size_t i) const { return m_VkCommandBuffers[i]; }

        VkCommandBuffer Begin(size_t i) const;
        void End(size_t) const;

    private:
        std::vector<VkCommandBuffer> m_VkCommandBuffers;

        const Device &m_Device;
        const CommandPool &m_CommandPool;

    };

}
