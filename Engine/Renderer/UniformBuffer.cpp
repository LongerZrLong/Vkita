#include "UniformBuffer.h"

#include "Renderer.h"
#include "GraphicsContext.h"

namespace VKT {

    UniformBuffer::UniformBuffer()
    {
        const auto bufferSize = sizeof(UniformBufferObject);

        m_Buffer = CreateScope<Vulkan::Buffer>(Renderer::GetGraphicsContext().GetDevice(), bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        m_DeviceMemory = CreateScope<Vulkan::DeviceMemory>(m_Buffer->AllocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
    }

    UniformBuffer::UniformBuffer(UniformBuffer &&other) noexcept
        : m_Buffer(other.m_Buffer.release()), m_DeviceMemory(other.m_DeviceMemory.release())
    {
    }

    void UniformBuffer::Update(const UniformBufferObject &ubo)
    {
        const auto data = m_DeviceMemory->Map(0, sizeof(UniformBufferObject));
        std::memcpy(data, &ubo, sizeof(ubo));
        m_DeviceMemory->Unmap();
    }

}
