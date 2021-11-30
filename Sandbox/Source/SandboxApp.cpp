#include <Vkita.h>

class ExampleLayer : public VKT::Layer
{
public:
    void OnUpdate(VKT::Timestep ts) override
    {
        if (VKT::Input::IsKeyPressed(VKT::Key::A))
        {
            VKT_INFO("{0} pressed", VKT::Key::A);
        }
    }

    void OnEvent(VKT::Event &event) override
    {
        VKT_INFO(event);
    }
};

class SandboxApp : public VKT::Application
{
public:
    SandboxApp()
    {
        PushLayer(new ExampleLayer());
    }

    ~SandboxApp() override = default;
};

VKT::Application *VKT::CreateApplication()
{
    Log::Init();
    return new SandboxApp();
}
