#pragma once

#include "Common.h"

namespace VKT::Vulkan {

    class Device;

    class Fence
    {
    public:
        Fence(const Fence&) = delete;
        Fence &operator = (const Fence&) = delete;
        Fence &operator = (Fence&&) = delete;

        explicit Fence(const Device &device, bool signaled);
        Fence(Fence &&other) noexcept;
        ~Fence();

        const Device &GetDevice() const { return m_Device; }
        const VkFence &GetVkHandle() const { return m_VkFence; }

        void Reset();
        void Wait(uint64_t timeout) const;

    private:
        VkFence m_VkFence{};

        const Device &m_Device;

    };

}
