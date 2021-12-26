#include "Config.h"

namespace VKT {

    const char *Config::kName = "Vkita";

    const uint32_t Config::kWidth = 800;
    const uint32_t Config::kHeight = 600;

    const bool Config::kEnableValidationLayers = true;

    const uint32_t Config::kMaxInFlightFrameCount = 2;
    const VkPresentModeKHR Config::kVkPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;

}