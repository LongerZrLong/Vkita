#include "App.h"

#include "Core/Log.h"

#include "Game/GameLogic.h"

namespace VKT {

    int App::Initialize()
    {
        Log::Init();

        m_Window = CreateScope<Window>();
        m_Window->SetEventCallback([this](Event &event) { App::OnEvent(event); });

        return 0;
    }

    void App::ShutDown()
    {

    }

    void App::Tick()
    {
        if (!m_Minimized)
        {
            g_GameLogic->OnUpdate();
        }
    }

    void App::OnEvent(Event &event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent &event) -> bool { return App::OnWindowClose(event); });
        dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent &event) -> bool { return App::OnWindowResize(event); });

        g_GameLogic->OnEvent(event);
    }

    bool App::OnWindowClose(WindowCloseEvent &event)
    {
        m_Running = false;
        return true;
    }

    bool App::OnWindowResize(WindowResizeEvent &event)
    {
        if (event.GetWidth() == 0 || event.GetHeight() == 0)
            m_Minimized = true;
        else
            m_Minimized = false;

        return false;
    }
}