#pragma once

#include <vector>

#include "Common.h"
#include "DebugUtils.h"

namespace VKT::Vulkan {

    class Surface;

    class Device
    {
    public:
        NON_COPIABLE(Device);

        Device(
            VkPhysicalDevice vkPhysicalDevice,
            const Surface &surface,
            const std::vector<const char*> &requiredExtensions,
            const VkPhysicalDeviceFeatures &vkPhysicalDeviceFeatures,
            const void *nextDeviceFeatures);

        ~Device();

        VkPhysicalDevice GetVkPhysicalDevice() const { return m_VkPhysicalDevice; }
        const Surface &GetSurface() const { return m_Surface; }

        const DebugUtils &GetDebugUtils() const { return m_DebugUtils; }

        uint32_t GetGraphicsFamilyIndex() const { return m_GraphicsFamilyIndex; }
        uint32_t GetPresentFamilyIndex() const { return m_PresentFamilyIndex; }
        uint32_t GetComputeFamilyIndex() const { return m_ComputeFamilyIndex; }
        uint32_t GetTransferFamilyIndex() const { return m_TransferFamilyIndex; }

        VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
        VkQueue GetPresentQueue() const { return m_PresentQueue; }
        VkQueue GetComputeQueue() const { return m_ComputeQueue; }
        VkQueue GetTransferQueue() const { return m_TransferQueue; }

        void WaitIdle() const;

    private:
        void CheckRequiredExtensions(VkPhysicalDevice physicalDevice, const std::vector<const char*> &requiredExtensions) const;

    private:
        VULKAN_HANDLE(VkDevice, m_VkDevice);

        const VkPhysicalDevice m_VkPhysicalDevice;
        const Surface &m_Surface;

        DebugUtils m_DebugUtils;

        uint32_t m_GraphicsFamilyIndex{};
        uint32_t m_PresentFamilyIndex{};
        uint32_t m_ComputeFamilyIndex{};
        uint32_t m_TransferFamilyIndex{};

        VkQueue m_GraphicsQueue{};
        VkQueue m_PresentQueue{};
        VkQueue m_ComputeQueue{};
        VkQueue m_TransferQueue{};

    };

}
