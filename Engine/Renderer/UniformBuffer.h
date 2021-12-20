#pragma once

#include <memory>

#include "Common/Base.h"

#include "Math/Glm.h"

#include "Vulkan/Buffer.h"
#include "Vulkan/Device.h"
#include "Vulkan/DeviceMemory.h"

namespace VKT {

    struct UniformBufferObject
    {
        alignas(16) glm::mat4 Model;
        alignas(16) glm::mat4 View;
        alignas(16) glm::mat4 Proj;
    };

    class UniformBuffer
    {
    public:
        UniformBuffer(const UniformBuffer&) = delete;
        UniformBuffer &operator = (const UniformBuffer&) = delete;
        UniformBuffer &operator = (UniformBuffer&&) = delete;

        UniformBuffer();
        UniformBuffer(UniformBuffer &&other) noexcept;
        ~UniformBuffer() = default;

        const Vulkan::Buffer &GetBuffer() const { return *m_Buffer; }

        void Update(const UniformBufferObject &ubo);

    private:
        Scope<Vulkan::Buffer> m_Buffer;
        Scope<Vulkan::DeviceMemory> m_DeviceMemory;

    };

}