#include "Fence.h"

#include "Device.h"

namespace VKT::Vulkan {

    Fence::Fence(const Device &device, const bool signaled)
        : m_Device(device)
    {
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

        Check(vkCreateFence(device.GetVkHandle(), &fenceInfo, nullptr, &m_VkFence),
              "create fence");
    }

    Fence::Fence(Fence &&other) noexcept
        : m_Device(other.m_Device), m_VkFence(other.m_VkFence)
    {
        other.m_VkFence = nullptr;
    }

    Fence::~Fence()
    {
        if (m_VkFence != nullptr)
        {
            vkDestroyFence(m_Device.GetVkHandle(), m_VkFence, nullptr);
            m_VkFence = nullptr;
        }
    }

    void Fence::Reset()
    {
        Check(vkResetFences(m_Device.GetVkHandle(), 1, &m_VkFence),
              "reset fence");
    }

    void Fence::Wait(const uint64_t timeout) const
    {
        Check(vkWaitForFences(m_Device.GetVkHandle(), 1, &m_VkFence, VK_TRUE, timeout),
              "wait for fence");
    }

}
