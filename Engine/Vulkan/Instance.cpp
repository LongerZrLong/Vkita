#include "Instance.h"

#include <sstream>

#include "Enumerate.h"
#include "Version.h"

namespace VKT::Vulkan {

    Instance::Instance(class Window &window, const std::vector<const char*> &validationLayers, uint32_t vulkanVersion)
        : m_Window(window)
    {
        // Check the minimum version.
        CheckVulkanMinimumVersion(vulkanVersion);

        // Get the list of required extensions.
        auto extensions = GetRequiredExtensions();

        // Check the validation layers and add them to the list of required extensions.
        CheckVulkanValidationLayerSupport(validationLayers);

        if (!validationLayers.empty())
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        // Create the Vulkan instance.
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Vkita";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = vulkanVersion;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        Check(vkCreateInstance(&createInfo, nullptr, &m_VkInstance));

        GetVulkanPhysicalDevices();
        GetVulkanLayers();
        GetVulkanExtensions();
    }

    Instance::~Instance()
    {
        if (m_VkInstance != nullptr)
        {
            vkDestroyInstance(m_VkInstance, nullptr);
            m_VkInstance = nullptr;
        }
    }

    std::vector<const char*> Instance::GetRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef __APPLE__
        extensions.push_back("VK_KHR_get_physical_device_properties2");
#endif

        return extensions;
    }

    void Instance::GetVulkanExtensions()
    {
        Vulkan::GetEnumerateVector(static_cast<const char*>(nullptr), vkEnumerateInstanceExtensionProperties, m_VkExtensionProps);
    }

    void Instance::GetVulkanLayers()
    {
        Vulkan::GetEnumerateVector(vkEnumerateInstanceLayerProperties, m_VkLayerProps);
    }

    void Instance::GetVulkanPhysicalDevices()
    {
        Vulkan::GetEnumerateVector(m_VkInstance, vkEnumeratePhysicalDevices, m_VkPhysicalDevices);

        if (m_VkPhysicalDevices.empty())
        {
            throw std::runtime_error("found no Vulkan physical devices");
        }
    }

    void Instance::CheckVulkanMinimumVersion(const uint32_t minVersion)
    {
        uint32_t version;
        Check(vkEnumerateInstanceVersion(&version));

        if (minVersion > version)
        {
            std::ostringstream out;
            out << "minimum required version not found (required " << Version(minVersion);
            out << ", found " << Version(version) << ")";

            throw std::runtime_error(out.str());
        }
    }

    void Instance::CheckVulkanValidationLayerSupport(const std::vector<const char*> &validationLayers)
    {
        const auto availableLayers = GetEnumerateVector(vkEnumerateInstanceLayerProperties);

        for (const char *layer : validationLayers)
        {
            auto result = std::find_if(availableLayers.begin(), availableLayers.end(), [layer](const VkLayerProperties &layerProperties)
            {
                return strcmp(layer, layerProperties.layerName) == 0;
            });

            if (result == availableLayers.end())
            {
                throw std::runtime_error("could not find the requested validation layer: '" + std::string(layer) + "'");
            }
        }
    }

}