#pragma once

#include "Common.h"

namespace VKT::Vulkan {

    class Strings
    {
    public:
        NON_COPIABLE(Strings)

        Strings() = delete;
        ~Strings() = delete;

        static const char *DeviceType(VkPhysicalDeviceType deviceType);
        static const char *VendorId(uint32_t vendorId);
    };

}
