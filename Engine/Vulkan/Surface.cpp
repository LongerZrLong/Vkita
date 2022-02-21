#include "Surface.h"

#include "Application/Window.h"

namespace VKT::Vulkan {

    Surface::Surface(const Instance &instance)
        : m_Instance(instance)
    {
        Check(glfwCreateWindowSurface(instance, (GLFWwindow *)instance.GetWindow().GetNativeWindow(), nullptr, &m_VkSurface));
    }

    Surface::~Surface()
    {
        if (m_VkSurface != nullptr)
        {
            vkDestroySurfaceKHR(m_Instance, m_VkSurface, nullptr);
            m_VkSurface = nullptr;
        }
    }

}
