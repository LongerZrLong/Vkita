#pragma once

#include "Common.h"

namespace VKT::Vulkan {

    class Device;

    class CommandPool
    {
    public:
        NON_COPIABLE(CommandPool);

        CommandPool(const Device &device, uint32_t queueFamilyIndex, bool allowReset);
        ~CommandPool();

        const Device &GetDevice() const { return m_Device; }

    private:
        VULKAN_HANDLE(VkCommandPool, m_VkCommandPool);

        const Device &m_Device;

    };

}
