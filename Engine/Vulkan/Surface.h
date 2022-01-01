#pragma once

#include "Common.h"
#include "Instance.h"

namespace VKT::Vulkan {

    class Surface
    {
    public:
        NON_COPIABLE(Surface);

        explicit Surface(const Instance &instance);
        ~Surface();

        const Instance &GetInstance() const { return m_Instance; }

    private:
        VULKAN_HANDLE(VkSurfaceKHR, m_VkSurface);

        const Instance& m_Instance;

    };

}
