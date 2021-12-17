#include "Application.h"

#include <GLFW/glfw3.h>

#include "Debug/Assert.h"

#include "Renderer/Renderer.h"

namespace VKT {

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    Application *Application::s_Instance = nullptr;

    Application::Application(const std::string &name)
    {
        VKT_CORE_ASSERT(!s_Instance, "Application already exists!");

        s_Instance = this;

        m_Window = CreateScope<Window>(WindowProps(name));
        m_Window->SetEventCallback([this](Event &event) { Application::OnEvent(event); });

        m_GraphicsContext = CreateScope<GraphicsContext>(*m_Window, VK_PRESENT_MODE_MAILBOX_KHR, enableValidationLayers);

        Renderer::Init(m_GraphicsContext.get());

    }

    void Application::OnEvent(Event &event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(
            [this](WindowCloseEvent &e) -> bool { return Application::OnWindowClose(e); });
        dispatcher.Dispatch<WindowResizeEvent>(
            [this](WindowResizeEvent &e) -> bool { return Application::OnWindowResize(e); });

        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
        {
            --it;
            (*it)->OnEvent(event);

            if (event.Handled) break;
        }
    }

    void Application::PushLayer(Layer *layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer *overlay)
    {
        m_LayerStack.PushOverlay(overlay);
        overlay->OnAttach();
    }

    void Application::Run()
    {
        while (m_Running)
        {
            auto time = (float) glfwGetTime();
            Timestep ts(time - m_Time);
            m_Time = time;

            if (!m_GraphicsContext->DrawFrameBegin())
            {
                m_Window->OnUpdate(ts);
                continue;
            }

            // Recording commands within OnUpdate of each layer
            if (!m_Minimized)
            {
                for (auto layer : m_LayerStack)
                {
                    layer->OnUpdate(ts);
                }
            }

            m_GraphicsContext->DrawFrameEnd();

            m_Window->OnUpdate(ts);
        }

        m_GraphicsContext->DeviceWaitIdle();
    }

    void Application::Close()
    {
        m_Running = false;
    }

    bool Application::OnWindowClose(WindowCloseEvent &event)
    {
        m_Running = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent &event)
    {
        if (event.GetWidth() == 0 || event.GetHeight() == 0) m_Minimized = true;
        else m_Minimized = false;

        Renderer::OnWindowResize(event.GetWidth(), event.GetHeight());

        return false;
    }
}
