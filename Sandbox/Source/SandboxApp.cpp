#include <Vkita.h>

namespace VKT {

    class ExampleLayer : public Layer
    {
    public:
        void OnUpdate(Timestep ts)
        {
            if (Input::IsKeyPressed(Key::A))
                VKT_INFO("{} is pressed.", char(Key::A));
        }

        void OnEvent(Event &e)
        {
        }

    };

    class SandboxApp : public Application
    {
    public:
        SandboxApp()
        {
            PushLayer(new ExampleLayer());
        }
    };
}

VKT::Application *VKT::g_App = new SandboxApp();
