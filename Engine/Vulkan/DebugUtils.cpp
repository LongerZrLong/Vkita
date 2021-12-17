#include "DebugUtils.h"

#include <iostream>

namespace VKT::Vulkan {
	
    DebugUtils::DebugUtils(VkInstance vkInstance)
        : vkSetDebugUtilsObjectNameEXT_(reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(vkInstance, "vkSetDebugUtilsObjectNameEXT")))
    {
#ifndef NDEBUG
        if (vkSetDebugUtilsObjectNameEXT_ == nullptr)
        {
            throw std::runtime_error("failed to get address of 'vkSetDebugUtilsObjectNameEXT'");
        }
#endif
    }

}