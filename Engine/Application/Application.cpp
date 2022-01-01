#include "Application.h"

#include <GLFW/glfw3.h>

#include "Core/Log.h"

namespace VKT {

    int Application::Initialize()
    {
        Log::Init();

        m_Window = CreateScope<Window>();
        m_Window->SetEventCallback([this](Event &event) { Application::OnEvent(event); });

        return 0;
    }

    void Application::ShutDown()
    {

    }

    void Application::Tick()
    {
        auto time = (float) glfwGetTime();
        Timestep ts(time - m_Time);
        m_Time = time;

        if (!m_Minimized)
        {
            for (auto layer : m_LayerStack)
            {
                layer->OnUpdate(ts);
            }
        }
    }

    void Application::OnEvent(Event &event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent &e) -> bool { return Application::OnWindowClose(e); });
        dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent &e) -> bool { return Application::OnWindowResize(e); });

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

    bool Application::OnWindowClose(WindowCloseEvent &event)
    {
        m_Running = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent &event)
    {
        if (event.GetWidth() == 0 || event.GetHeight() == 0)
            m_Minimized = true;
        else
            m_Minimized = false;

        return false;
    }
}