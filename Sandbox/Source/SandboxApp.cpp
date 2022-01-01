#include <Vkita.h>

#include <Core/InputManager.h>
#include <Core/FileSystem.h>
#include <Core/GraphicsManager.h>

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

    Application         *g_App = new SandboxApp();
    FileSystem          *g_FileSystem = new FileSystem();
    InputManager        *g_InputManager = new InputManager();
    GraphicsManager     *g_GraphicsManager = new GraphicsManager();
}

using namespace VKT;

int main(int, char**)
{
    g_App->Initialize();
    g_FileSystem->Initialize();
    g_InputManager->Initialize();
    g_GraphicsManager->Initialize();

    while (g_App->IsRunning())
    {
        g_App->Tick();
        g_InputManager->Tick();
        g_GraphicsManager->Tick();
    }

    g_InputManager->ShutDown();
    g_GraphicsManager->ShutDown();
    g_App->ShutDown();

    return 0;
}
