#include "Surface.h"

#include "Application/Window.h"

namespace VKT::Vulkan {

    Surface::Surface(const Instance &instance)
        : m_Instance(instance)
    {
        Check(glfwCreateWindowSurface(instance.GetVkHandle(), (GLFWwindow *)instance.GetWindow().GetNativeWindow(), nullptr, &m_VkSurface));
    }

    Surface::~Surface()
    {
        if (m_VkSurface != nullptr)
        {
            vkDestroySurfaceKHR(m_Instance.GetVkHandle(), m_VkSurface, nullptr);
            m_VkSurface = nullptr;
        }
    }

}
