#pragma once

#include "ApplicationEvent.h"
#include "LayerStack.h"
#include "Window.h"

#include "Core/Base.h"

#include "Interface/IRuntimeModule.h"

namespace VKT {

    class Application : public IRuntimeModule
    {
    public:
        int Initialize() final;
        void ShutDown() final;
        void Tick() final;

        virtual bool IsRunning() { return m_Running; }
        virtual bool IsMinimized() { return m_Minimized; }

        Window &GetWindow() { return *m_Window; }

        void OnEvent(Event &e);

        void PushLayer(Layer *layer);
        void PushOverlay(Layer *overlay);

    private:
        bool OnWindowClose(WindowCloseEvent &e);
        bool OnWindowResize(WindowResizeEvent &e);

    private:
        Scope<Window> m_Window;

        LayerStack m_LayerStack;

        bool m_Running = true;
        bool m_Minimized = false;

        float m_Time;
    };

    extern Application *g_App;

}