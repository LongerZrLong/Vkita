#pragma once

#include "Common.h"

namespace VKT::Vulkan {

    class Instance;

    class DebugUtilsMessenger
    {
    public:
        NON_COPIABLE(DebugUtilsMessenger);

        DebugUtilsMessenger(const Instance &instance, VkDebugUtilsMessageSeverityFlagBitsEXT threshold);
        ~DebugUtilsMessenger();

        VkDebugUtilsMessageSeverityFlagBitsEXT GetThreshold() const { return m_Threshold; }

    private:
        VULKAN_HANDLE(VkDebugUtilsMessengerEXT, m_VkDebugUtilsMessenger);

        const Instance &m_Instance;
        const VkDebugUtilsMessageSeverityFlagBitsEXT m_Threshold;

    };

}
