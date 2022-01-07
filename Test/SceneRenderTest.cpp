#include <Vkita.h>

#include <Core/FileSystem.h>
#include <Core/SceneManager.h>
#include <Core/GraphicsManager.h>
#include <Core/InputManager.h>
#include <Core/PhysicsManager.h>

namespace VKT {

    class SceneRenderTestGameLogic : public GameLogic
    {
    public:

        int Initialize()
        {
            g_SceneManager->LoadScene(g_FileSystem->Append(g_FileSystem->GetRoot(), "Resource/Scenes/nanosuit/nanosuit.obj"));

            return 0;
        }
    };
}

namespace VKT {

    App             *g_App = new App();
    FileSystem      *g_FileSystem = new FileSystem();
    SceneManager    *g_SceneManager = new SceneManager();
    InputManager    *g_InputManager = new InputManager();
    GraphicsManager *g_GraphicsManager = new GraphicsManager();
    PhysicsManager  *g_PhysicsManager = new PhysicsManager();
    GameLogic       *g_GameLogic = new SceneRenderTestGameLogic();
}

using namespace VKT;

int main()
{
    g_App->Initialize();
    g_InputManager->Initialize();
    g_FileSystem->Initialize();
    g_SceneManager->Initialize();
    g_PhysicsManager->Initialize();
    g_GraphicsManager->Initialize();
    g_GameLogic->Initialize();

    while (g_App->IsRunning())
    {
        g_App->Tick();
        g_FileSystem->Tick();
        g_InputManager->Tick();
        g_SceneManager->Tick();
        g_PhysicsManager->Initialize();
        g_GraphicsManager->Tick();
        g_GameLogic->Tick();
    }

    g_GameLogic->Tick();
    g_GraphicsManager->ShutDown();
    g_PhysicsManager->Initialize();
    g_InputManager->ShutDown();
    g_SceneManager->ShutDown();
    g_FileSystem->ShutDown();
    g_App->ShutDown();

    return 0;
}