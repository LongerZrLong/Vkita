#include <Vkita.h>

#include <Core/FileSystem.h>
#include <Core/SceneManager.h>
#include <Core/GraphicsManager.h>
#include <Core/InputManager.h>
#include <Core/PhysicsManager.h>
#include <Core/DebugManager.h>

namespace VKT {

    class SceneRenderTestGameLogic : public GameLogic
    {
    public:

        int Initialize()
        {
            g_SceneManager->LoadScene(g_FileSystem->Append(g_FileSystem->GetRoot(), "Resource/Scenes/nanosuit/nanosuit.obj"));

            SceneNode &rootNode = g_SceneManager->GetScene().m_SceneNodes[0];

            // x axis
            g_DebugManager->AddLine(rootNode, {-1000.f, 0.0f, 0.0f}, {1000.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});

            // y axis
            g_DebugManager->AddLine(rootNode, {0.f, -1000.0f, 0.0f}, {0.0f, 1000.0f, 0.0f}, {0.0f, 1.0f, 0.0f});

            // z axis
            g_DebugManager->AddLine(rootNode, {0.f, 0.0f, -1000.0f}, {0.0f, 0.0f, 1000.0f}, {0.0f, 0.0f, 1.0f});

            return 0;
        }

        void Tick()
        {
            static bool prevReleased = true;

            if (g_InputManager->IsKeyPressed(Key::D))
            {
                if (prevReleased)
                    g_DebugManager->ToggleDebugInfo();

                prevReleased = false;
                return;
            }

            prevReleased = true;
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
    DebugManager    *g_DebugManager = new DebugManager();
}

using namespace VKT;

int main()
{
    g_App->Initialize();
    g_FileSystem->Initialize();
    g_InputManager->Initialize();
    g_SceneManager->Initialize();
    g_PhysicsManager->Initialize();
    g_GraphicsManager->Initialize();
    g_GameLogic->Initialize();
    g_DebugManager->Initialize();

    while (g_App->IsRunning())
    {
        g_App->Tick();
        g_FileSystem->Tick();
        g_InputManager->Tick();
        g_SceneManager->Tick();
        g_PhysicsManager->Initialize();
        g_GraphicsManager->Tick();
        g_GameLogic->Tick();
        g_DebugManager->Tick();
    }

    g_DebugManager->ShutDown();
    g_GameLogic->ShutDown();
    g_GraphicsManager->ShutDown();
    g_PhysicsManager->Initialize();
    g_InputManager->ShutDown();
    g_SceneManager->ShutDown();
    g_FileSystem->ShutDown();
    g_App->ShutDown();

    return 0;
}