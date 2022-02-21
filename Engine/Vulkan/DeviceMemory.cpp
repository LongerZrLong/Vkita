#include "DeviceMemory.h"

namespace VKT::Vulkan {

    DeviceMemory::DeviceMemory(
        const Device &device,
        const size_t size,
        const uint32_t memoryTypeBits,
        const VkMemoryAllocateFlags allocateFlags,
        const VkMemoryPropertyFlags propertyFlags) :
        m_Device(device)
    {
        VkMemoryAllocateFlagsInfo flagsInfo = {};
        flagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
        flagsInfo.pNext = nullptr;
        flagsInfo.flags = allocateFlags;

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = &flagsInfo;
        allocInfo.allocationSize = size;
        allocInfo.memoryTypeIndex = FindMemoryType(memoryTypeBits, propertyFlags);

        Check(vkAllocateMemory(device, &allocInfo, nullptr, &m_VkDeviceMemory));
    }

    DeviceMemory::DeviceMemory(DeviceMemory &&other) noexcept
        : m_Device(other.m_Device), m_VkDeviceMemory(other.m_VkDeviceMemory)
    {
        other.m_VkDeviceMemory = nullptr;
    }

    DeviceMemory::~DeviceMemory()
    {
        if (m_VkDeviceMemory != nullptr)
        {
            vkFreeMemory(m_Device, m_VkDeviceMemory, nullptr);
            m_VkDeviceMemory = nullptr;
        }
    }

    void *DeviceMemory::Map(const size_t offset, const size_t size)
    {
        void *data;
        Check(vkMapMemory(m_Device, m_VkDeviceMemory, offset, size, 0, &data));
        return data;
    }

    void DeviceMemory::Unmap()
    {
        vkUnmapMemory(m_Device, m_VkDeviceMemory);
    }

    uint32_t DeviceMemory::FindMemoryType(const uint32_t typeFilter, const VkMemoryPropertyFlags propertyFlags) const
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(m_Device.GetVkPhysicalDevice(), &memProperties);

        for (uint32_t i = 0; i != memProperties.memoryTypeCount; ++i)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags)
            {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type");
    }

}
