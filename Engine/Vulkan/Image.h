#pragma once

#include "Common.h"

namespace VKT::Vulkan {

	class Device;
	class Buffer;
	class CommandPool;
	class DeviceMemory;

	class Image
	{
	public:
        Image(const Device &device, VkExtent2D extent, VkFormat format);
        Image(const Device &device, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags);
        Image(const Image&) = delete;
        Image(Image&& other) noexcept;

        ~Image();

		Image& operator = (const Image&) = delete;
		Image& operator = (Image&&) = delete;

		VkExtent2D GetVkExtent2D() const { return m_VkExtent2D; }
		VkFormat GetVkFormat() const { return m_VkFormat; }

		DeviceMemory AllocateMemory(VkMemoryPropertyFlags propertyFlags) const;
		VkMemoryRequirements GetVkMemoryRequirements() const;

		void TransitionImageLayout(const Device &device, const CommandPool &commandPool, VkImageLayout newLayout);
		void CopyFrom(const Device &device, const CommandPool &commandPool, const Buffer &buffer);

	private:
        VULKAN_HANDLE(VkImage, m_VkImage);

        const VkExtent2D m_VkExtent2D;
        const VkFormat m_VkFormat;
        VkImageLayout m_VkImageLayout;
        VkImageTiling m_VkImageTiling;

		const Device &m_Device;

	};

}
