#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Common/Base.h"

#define VULKAN_HANDLE(VulkanHandleType, name) \
public: \
	VulkanHandleType GetVkHandle() const { return name; } \
private: \
	VulkanHandleType name{};

namespace VKT::Vulkan
{
    void Check(VkResult result, const char* operation);
    const char* ToString(VkResult result);
}
