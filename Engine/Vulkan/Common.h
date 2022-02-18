#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Core/Base.h"

// TODO: Change GetVkHandle to type conversion operator
#define VULKAN_HANDLE(VulkanHandleType, name) \
public: \
	const VulkanHandleType &GetVkHandle() const { return name; } \
private: \
	VulkanHandleType name{VK_NULL_HANDLE};

namespace VKT::Vulkan
{
    void Check(VkResult result);
    const char* ToString(VkResult result);
}
