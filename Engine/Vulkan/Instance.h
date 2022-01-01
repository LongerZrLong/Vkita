#pragma once

#include <vector>

#include "Common.h"

#include "Application/Window.h"

namespace VKT::Vulkan {

    class Instance
    {
    public:
        NON_COPIABLE(Instance);

        Instance(Window &window, const std::vector<const char*> &validationLayers, uint32_t vulkanVersion);
        ~Instance();

        const Window &GetWindow() const { return m_Window; }

        const std::vector<VkExtensionProperties> &GetVkExtensionProps() const { return m_VkExtensionProps; }
        const std::vector<VkLayerProperties> &GetVkLayerProps() const { return m_VkLayerProps; }
        const std::vector<VkPhysicalDevice> &GetVkPhysicalDevices() const { return m_VkPhysicalDevices; }
        const std::vector<const char*> &GetValidationLayers() const { return m_ValidationLayers; }

    private:
        std::vector<const char*> GetRequiredExtensions();

        void GetVulkanExtensions();
        void GetVulkanLayers();
        void GetVulkanPhysicalDevices();

        static void CheckVulkanMinimumVersion(uint32_t minVersion);
        static void CheckVulkanValidationLayerSupport(const std::vector<const char*> &validationLayers);

    private:
        VULKAN_HANDLE(VkInstance, m_VkInstance);

        Window &m_Window;
        const std::vector<const char*> m_ValidationLayers;

        std::vector<VkExtensionProperties> m_VkExtensionProps;
        std::vector<VkLayerProperties> m_VkLayerProps;
        std::vector<VkPhysicalDevice> m_VkPhysicalDevices;

    };
}