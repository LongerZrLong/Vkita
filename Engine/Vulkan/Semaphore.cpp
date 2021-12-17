#include "Semaphore.h"

#include "Device.h"

namespace VKT::Vulkan {

    Semaphore::Semaphore(const Device& device)
        : m_Device(device)
    {
        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        Check(vkCreateSemaphore(device.GetVkHandle(), &semaphoreInfo, nullptr, &m_VkSemaphore),
              "create semaphores");
    }

    Semaphore::Semaphore(Semaphore&& other) noexcept
        : m_Device(other.m_Device), m_VkSemaphore(other.m_VkSemaphore)
    {
        other.m_VkSemaphore = nullptr;
    }

    Semaphore::~Semaphore()
    {
        if (m_VkSemaphore != nullptr)
        {
            vkDestroySemaphore(m_Device.GetVkHandle(), m_VkSemaphore, nullptr);
            m_VkSemaphore = nullptr;
        }
    }

}
