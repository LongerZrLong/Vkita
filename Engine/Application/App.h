#pragma once

#include "Window.h"

#include "Core/Base.h"

#include "Event/ApplicationEvent.h"

#include "Interface/IRuntimeModule.h"

namespace VKT {

    class App : public IRuntimeModule
    {
    public:
        int Initialize() final;
        void ShutDown() final;
        void Tick() final;

        bool IsRunning() { return m_Running; }
        bool IsMinimized() { return m_Minimized; }

        Window &GetWindow() { return *m_Window; }

    private:
        void OnEvent(Event &event);

        bool OnWindowClose(WindowCloseEvent &event);
        bool OnWindowResize(WindowResizeEvent &event);

    private:
        Scope<Window> m_Window;

        bool m_Running = true;
        bool m_Minimized = false;

        float m_Time;
    };

    extern App *g_App;

}