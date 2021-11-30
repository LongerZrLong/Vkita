#pragma once

#include <string>

#include <GLFW/glfw3.h>

#include "Events/Event.h"
#include "Core/Timestep.h"

namespace VKT {

    struct WindowProps
    {
        std::string Title;
        uint32_t Width;
        uint32_t Height;

        explicit WindowProps(std::string title = "Vkita", uint32_t width = 800, uint32_t height = 600)
            : Title(std::move(title)), Width(width), Height(height) {}
    };

    class Window
    {
    public:
        using EventCallbackFn = std::function<void(Event &)>;

        explicit Window(const WindowProps &props);
        ~Window();

        void OnUpdate(Timestep ts);

        uint32_t GetWidth() const { return m_Data.Width; };
        uint32_t GetHeight() const { return m_Data.Height; };

        // Window attributes
        void SetVSync(bool enabled);
        bool IsVSync() const { return m_Data.VSync; };
        void SetEventCallback(const EventCallbackFn &callback) { m_Data.EventCallback = callback; }

        void *GetNativeWindow() const { return m_Window; }

    private:
        void Init(const WindowProps &props);
        void Shutdown();

    private:
        GLFWwindow *m_Window;

        struct WindowData
        {
            std::string Title;
            uint32_t Width, Height;
            bool VSync;

            EventCallbackFn EventCallback;
        };

        WindowData m_Data;
    };
}