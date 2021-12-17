#include "ExamplerLayer.h"

#include <Vkita.h>

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
