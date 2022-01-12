#include "Context.h"

#include "Core/Log.h"

#include "Application/App.h"

#include "Vulkan/Enumerate.h"
#include "Vulkan/Version.h"
#include "Vulkan/Strings.h"

namespace VKT::Rendering {

    void Context::Initialize()
    {
        const auto validationLayers = Config::kEnableValidationLayers
                                      ? std::vector<const char*>{ "VK_LAYER_KHRONOS_validation" }
                                      : std::vector<const char*>();

        instance = CreateScope<Vulkan::Instance>(g_App->GetWindow(), validationLayers, VK_API_VERSION_1_2);
        debugUtilsMessenger = Config::kEnableValidationLayers?
                                CreateScope<Vulkan::DebugUtilsMessenger>(*instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) : nullptr;
        surface = CreateScope<Vulkan::Surface>(*instance);

        CreateDevice();

        // Create swap chain and command buffers
        CreateSwapChain();

        CreateSyncObjects();

        LogVulkanInfo();
    }

    void Context::ShutDown()
    {
        device->WaitIdle();

        DeleteSwapChain();
        DeleteSyncObjects();
    }

    const std::vector<VkExtensionProperties> &Context::GetExtensions() const
    {
        return instance->GetVkExtensionProps();
    }

    const std::vector<VkLayerProperties> &Context::GetLayers() const
    {
        return instance->GetVkLayerProps();
    }

    const std::vector<VkPhysicalDevice> &Context::GetPhysicalDevices() const
    {
        return instance->GetVkPhysicalDevices();
    }

    void Context::SetPhysicalDevice(VkPhysicalDevice physicalDevice)
    {
        if (device)
        {
            throw std::logic_error("physical device has already been set");
        }

        std::vector<const char*> requiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef __APPLE__
        requiredExtensions.push_back("VK_KHR_portability_subset");
#endif

        VkPhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        deviceFeatures.sampleRateShading = VK_TRUE;

        // If the non-solid fill modes feature is not enabled, polygonMode must be VK_POLYGON_MODE_FILL
        deviceFeatures.fillModeNonSolid = VK_TRUE;

        device = CreateScope<Vulkan::Device>(physicalDevice, *surface, requiredExtensions, deviceFeatures, nullptr);
        cmdPool = CreateScope<Vulkan::CommandPool>(*device, device->GetGraphicsFamilyIndex(), true);
    }

    void Context::CreateDevice()
    {
        const auto &physicalDevices = GetPhysicalDevices();
        const auto result = std::find_if(physicalDevices.begin(), physicalDevices.end(), [](const VkPhysicalDevice &device)
        {
            // We want a device with a graphics queue.
            const auto queueFamilies = Vulkan::GetEnumerateVector(device, vkGetPhysicalDeviceQueueFamilyProperties);
            const auto hasGraphicsQueue = std::find_if(queueFamilies.begin(), queueFamilies.end(), [](const VkQueueFamilyProperties& queueFamily)
            {
                return queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT;
            });

            return hasGraphicsQueue != queueFamilies.end();
        });

        if (result == physicalDevices.end())
        {
            throw std::runtime_error("cannot find a suitable device");
        }

        VkPhysicalDeviceProperties2 deviceProp{};
        deviceProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        vkGetPhysicalDeviceProperties2(*result, &deviceProp);

        SetPhysicalDevice(*result);
    }

    void Context::CreateSwapChain()
    {
        // Wait until the window is visible.
        while (g_App->IsMinimized())
        {
            g_App->GetWindow().WaitForEvents();
        }

        swapChain = CreateScope<Vulkan::SwapChain>(*device, Config::kVkPresentMode);
        depthBuffer = CreateScope<Vulkan::DepthBuffer>(*device, *cmdPool, swapChain->GetVkExtent2D());
        renderPass = CreateScope<Vulkan::RenderPass>(*device, *swapChain, *depthBuffer, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_LOAD_OP_CLEAR);

        for (const auto &imageView : swapChain->GetImageViews())
        {
            frameBuffers.emplace_back(CreateScope<Vulkan::FrameBuffer>(*device, *imageView, *renderPass));
        }

        cmdBuffers = CreateScope<Vulkan::CommandBuffers>(*device, *cmdPool, frameBuffers.size());

        imagesInFlight.resize(swapChain->GetVkImages().size(), VK_NULL_HANDLE);
    }

    void Context::DeleteSwapChain()
    {
        cmdBuffers.reset();
        frameBuffers.clear();
        renderPass.reset();
        depthBuffer.reset();
        swapChain.reset();
    }

    void Context::CreateSyncObjects()
    {
        imageAvailableSemaphores.resize(Config::kMaxInFlightFrameCount);
        renderFinishedSemaphores.resize(Config::kMaxInFlightFrameCount);
        inFlightFences.resize(Config::kMaxInFlightFrameCount);
        imagesInFlight.resize(swapChain->GetVkImages().size(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < Config::kMaxInFlightFrameCount; i++)
        {
            if (vkCreateSemaphore(device->GetVkHandle(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(device->GetVkHandle(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(device->GetVkHandle(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    void Context::DeleteSyncObjects()
    {
        for (size_t i = 0; i < Config::kMaxInFlightFrameCount; i++)
        {
            vkDestroySemaphore(device->GetVkHandle(), renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device->GetVkHandle(), imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device->GetVkHandle(), inFlightFences[i], nullptr);
        }
    }

    void Context::RecreateSwapChain()
    {
        device->WaitIdle();
        DeleteSwapChain();
        CreateSwapChain();
    }

    void Context::LogVulkanInfo()
    {
        VKT_CORE_INFO("Vulkan SDK Header Version: {}", VK_HEADER_VERSION);

//        VKT_CORE_INFO("Vulkan Instance Extensions: ");
//        for (const auto &extension : GetExtensions())
//            VKT_CORE_INFO("-  {} ({})", extension.extensionName, Vulkan::Version(extension.specVersion));
//
//        VKT_CORE_INFO("Vulkan Instance Layers: ");
//        for (const auto &layer : GetLayers())
//            VKT_CORE_INFO("-  {} ({}) : {}", layer.layerName, Vulkan::Version(layer.specVersion), layer.description);

        VKT_CORE_INFO("Vulkan Devices: ");
        for (const auto &physicalDevice : GetPhysicalDevices())
        {
            VkPhysicalDeviceDriverProperties driverProp{};
            driverProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;

            VkPhysicalDeviceProperties2 deviceProp{};
            deviceProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
            deviceProp.pNext = &driverProp;

            vkGetPhysicalDeviceProperties2(physicalDevice, &deviceProp);

            const auto &prop = deviceProp.properties;

            const Vulkan::Version vulkanVersion(prop.apiVersion);
            const Vulkan::Version driverVersion(prop.driverVersion, prop.vendorID);

            std::string chosen = physicalDevice == device->GetVkPhysicalDevice() ? "√" : " ";

            VKT_CORE_INFO("-  [{}] '{}' ({}: vulkan {}, driver {} {} - {})",
                          chosen,
                          prop.deviceName,
                          Vulkan::Strings::DeviceType(prop.deviceType),
                          vulkanVersion,
                          driverProp.driverName,
                          driverProp.driverInfo,
                          driverVersion);
        }
    }

}