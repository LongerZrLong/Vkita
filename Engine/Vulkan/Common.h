#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Core/Base.h"

#define VULKAN_HANDLE(VulkanHandleType, name) \
public: \
    operator VulkanHandleType() const { return name; }\
private: \
	VulkanHandleType name{VK_NULL_HANDLE};

namespace VKT::Vulkan
{
    void Check(VkResult result);
    const char* ToString(VkResult result);
}
