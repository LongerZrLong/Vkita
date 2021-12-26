#pragma once

#include <vulkan/vulkan.h>

namespace VKT {

    struct Config
    {
        static const char *kName;

        static const uint32_t kWidth;
        static const uint32_t kHeight;

        static const uint32_t kMaxInFlightFrameCount;
        static const VkPresentModeKHR kVkPresentMode;

    };

}