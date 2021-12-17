#pragma once

#include "Base.h"
#include "LayerStack.h"

#include "Events/ApplicationEvent.h"
#include "Window/Window.h"

#include "Renderer/GraphicsContext.h"

namespace VKT {

    class Application
    {
    public:
        explicit Application(const std::string &name = "App");
        virtual ~Application() = default;

        Application(const Application &) = delete;
        void operator=(const Application &) = delete;

        void OnEvent(Event &e);

        void PushLayer(Layer *layer);
        void PushOverlay(Layer *overlay);

        void Run();
        void Close();

        Window &GetWindow() { return *m_Window; }

        static Application &Get() { return *s_Instance; }

    private:
        bool OnWindowClose(WindowCloseEvent &e);
        bool OnWindowResize(WindowResizeEvent &e);

    private:
        Scope<Window> m_Window;
        Scope<GraphicsContext> m_GraphicsContext;

        LayerStack m_LayerStack;

        bool m_Running = true;
        bool m_Minimized = false;

        float m_Time = 0.0f;

    private:
        static Application *s_Instance;
    };


    // To be define in CLIENT side
    Application *CreateApplication();
}
