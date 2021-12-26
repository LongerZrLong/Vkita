#include "DebugUtils.h"

namespace VKT::Vulkan {
	
    DebugUtils::DebugUtils(VkInstance vkInstance)
        : vkSetDebugUtilsObjectNameEXT_(reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(vkInstance, "vkSetDebugUtilsObjectNameEXT")))
    {
        if (Config::kEnableValidationLayers && vkSetDebugUtilsObjectNameEXT_ == nullptr)
        {
            throw std::runtime_error("failed to get address of 'vkSetDebugUtilsObjectNameEXT'");
        }
    }

}