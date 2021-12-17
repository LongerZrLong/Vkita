#pragma once

#include "Common.h"

namespace VKT::Vulkan {

    class Device;

    class Semaphore
    {
    public:
        Semaphore(const Semaphore&) = delete;
        Semaphore &operator = (const Semaphore&) = delete;
        Semaphore &operator = (Semaphore&&) = delete;

        explicit Semaphore(const Device &device);
        Semaphore(Semaphore &&other) noexcept;
        ~Semaphore();

        const Device &GetDevice() const { return m_Device; }

    private:
        VULKAN_HANDLE(VkSemaphore, m_VkSemaphore);

        const Device &m_Device;

    };

}
