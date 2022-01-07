#pragma once

#include "Event/Event.h"

#include "Core/Timestep.h"

class GLFWwindow;

namespace VKT {

    class Window
    {
    public:
        using EventCallbackFn = std::function<void(Event &)>;

        Window();
        ~Window();

        uint32_t GetWidth() const { return m_Data.Width; };
        uint32_t GetHeight() const { return m_Data.Height; };

        // Window attributes
        void SetEventCallback(const EventCallbackFn &callback) { m_Data.EventCallback = callback; }

        void *GetNativeWindow() const { return m_Window; }

        void WaitForEvents();

    private:
        void Initialize();
        void ShutDown();

    private:
        GLFWwindow *m_Window;

        struct WindowData
        {
            const char *Title;
            uint32_t Width, Height;

            EventCallbackFn EventCallback;
        };

        WindowData m_Data;
    };
}