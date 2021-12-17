#pragma once

#include "Common.h"

namespace VKT::Vulkan {

    class Instance;

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
