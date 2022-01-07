#include <Vkita.h>

#include <Core/InputManager.h>

namespace VKT {

    class InputTestLayer : public Layer
    {
    public:
        void OnUpdate()
        {
            if (g_InputManager->IsKeyPressed(Key::A))
                VKT_INFO("Key A is pressed");
        }

        void OnEvent(Event &event)
        {
            VKT_TRACE(event);
        }
    };

    class InputTestGameLogic : public GameLogic
    {
    public:
        int Initialize()
        {
            PushLayer(new InputTestLayer());

            return 0;
        }
    };
}

namespace VKT {

    App             *g_App = new App();
    InputManager    *g_InputManager = new InputManager();
    GameLogic       *g_GameLogic = new InputTestGameLogic();
}

using namespace VKT;

int main()
{
    g_App->Initialize();
    g_InputManager->Initialize();
    g_GameLogic->Initialize();

    while (g_App->IsRunning())
    {
        g_App->Tick();
        g_InputManager->Tick();
        g_GameLogic->Tick();
    }

    g_GameLogic->ShutDown();
    g_InputManager->ShutDown();
    g_App->ShutDown();

    return 0;
}