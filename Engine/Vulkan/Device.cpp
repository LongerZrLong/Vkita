#include "Device.h"

#include <string>
#include <algorithm>
#include <set>

#include "Enumerate.h"

namespace VKT::Vulkan {

    namespace {
        
        std::vector<VkQueueFamilyProperties>::const_iterator FindQueue(
            const std::vector<VkQueueFamilyProperties> &vkQueueFamilyProps,
            const std::string &name,
            const VkQueueFlags requiredBits,
            const VkQueueFlags excludedBits)
        {
            const auto family = std::find_if(vkQueueFamilyProps.begin(), vkQueueFamilyProps.end(), [requiredBits, excludedBits](const VkQueueFamilyProperties& queueFamily)
            {
                return
                    queueFamily.queueCount > 0 &&
                    queueFamily.queueFlags & requiredBits &&
                    !(queueFamily.queueFlags & excludedBits);
            });

            if (family == vkQueueFamilyProps.end())
            {
                throw std::runtime_error("found no matching " + name + " queue");
            }

            return family;
        }
    }

    Device::Device(
        VkPhysicalDevice vkPhysicalDevice,
        const Surface &surface,
        const std::vector<const char*> &requiredExtensions,
        const VkPhysicalDeviceFeatures &deviceFeatures,
        const void *nextDeviceFeatures) :
        m_VkPhysicalDevice(vkPhysicalDevice),
        m_Surface(surface),
        m_DebugUtils(surface.GetInstance().GetVkHandle())
    {
        CheckRequiredExtensions(vkPhysicalDevice, requiredExtensions);

        const auto queueFamilies = GetEnumerateVector(vkPhysicalDevice, vkGetPhysicalDeviceQueueFamilyProperties);

        // Find the graphics queue.
        const auto graphicsFamily = FindQueue(queueFamilies, "graphics", VK_QUEUE_GRAPHICS_BIT, 0);
        const auto computeFamily = FindQueue(queueFamilies, "compute", VK_QUEUE_COMPUTE_BIT, 0);
        const auto transferFamily = FindQueue(queueFamilies, "transfer", VK_QUEUE_TRANSFER_BIT, 0);

        // Find the presentation queue (usually the same as graphics queue).
        const auto presentFamily = std::find_if(queueFamilies.begin(), queueFamilies.end(), [&](const VkQueueFamilyProperties& queueFamily)
        {
            VkBool32 presentSupport = false;
            const uint32_t i = static_cast<uint32_t>(&*queueFamilies.cbegin() - &queueFamily);
            vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, surface.GetVkHandle(), &presentSupport);
            return queueFamily.queueCount > 0 && presentSupport;
        });

        if (presentFamily == queueFamilies.end())
        {
            throw std::runtime_error("found no presentation queue");
        }

        m_GraphicsFamilyIndex = static_cast<uint32_t>(graphicsFamily - queueFamilies.begin());
        m_PresentFamilyIndex = static_cast<uint32_t>(presentFamily - queueFamilies.begin());
        m_ComputeFamilyIndex = static_cast<uint32_t>(computeFamily - queueFamilies.begin());
        m_TransferFamilyIndex = static_cast<uint32_t>(transferFamily - queueFamilies.begin());

        // Queues can be the same
        const std::set<uint32_t> uniqueQueueFamilies =
            {
                m_GraphicsFamilyIndex,
                m_PresentFamilyIndex,
                m_ComputeFamilyIndex,
                m_TransferFamilyIndex
            };

        // Create queues
        float queuePriority = 1.0f;
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

        for (uint32_t queueFamilyIndex : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;

            queueCreateInfos.push_back(queueCreateInfo);
        }

        // Create device
        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pNext = nextDeviceFeatures;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledLayerCount = static_cast<uint32_t>(m_Surface.GetInstance().GetValidationLayers().size());
        createInfo.ppEnabledLayerNames = m_Surface.GetInstance().GetValidationLayers().data();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();

        Check(vkCreateDevice(vkPhysicalDevice, &createInfo, nullptr, &m_VkDevice));

        m_DebugUtils.SetDevice(m_VkDevice);

        vkGetDeviceQueue(m_VkDevice, m_GraphicsFamilyIndex, 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_VkDevice, m_PresentFamilyIndex, 0, &m_PresentQueue);
        vkGetDeviceQueue(m_VkDevice, m_ComputeFamilyIndex, 0, &m_ComputeQueue);
        vkGetDeviceQueue(m_VkDevice, m_TransferFamilyIndex, 0, &m_TransferQueue);
    }

    Device::~Device()
    {
        if (m_VkDevice != nullptr)
        {
            vkDestroyDevice(m_VkDevice, nullptr);
            m_VkDevice = nullptr;
        }
    }

    void Device::WaitIdle() const
    {
        Check(vkDeviceWaitIdle(m_VkDevice));
    }

    void Device::CheckRequiredExtensions(VkPhysicalDevice physicalDevice, const std::vector<const char*> &requiredExtensions) const
    {
        const auto availableExtensions = GetEnumerateVector(physicalDevice, static_cast<const char*>(nullptr), vkEnumerateDeviceExtensionProperties);
        std::set<std::string> required(requiredExtensions.begin(), requiredExtensions.end());

        for (const auto &extension : availableExtensions)
        {
            required.erase(extension.extensionName);
        }

        if (!required.empty())
        {
            bool first = true;
            std::string extensions;

            for (const auto &extension : required)
            {
                if (!first)
                {
                    extensions += ", ";
                }

                extensions += extension;
                first = false;
            }

            throw std::runtime_error("missing required extensions: " + extensions);
        }
    }

}
