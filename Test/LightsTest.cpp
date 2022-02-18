#include <Vkita.h>

#include <Core/FileSystem.h>
#include <Core/SceneManager.h>
#include <Core/GraphicsManager.h>
#include <Core/InputManager.h>
#include <Core/PhysicsManager.h>
#include <Core/DebugManager.h>

namespace VKT {

    class EditorCameraTestLayer : public Layer
    {
    public:
        void OnAttach()
        {
            g_SceneManager->LoadScene(g_FileSystem->Append(g_FileSystem->GetRoot(), "Resource/Scenes/lights/lights.gltf"));

            // Manually set debug info
            {
                SceneNode &rootNode = g_SceneManager->GetScene().m_SceneNodes.front();

                // x axis
                g_DebugManager->AddLine(rootNode, {-1000.f, 0.0f, 0.0f}, {1000.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});

                // y axis
                g_DebugManager->AddLine(rootNode, {0.f, -1000.0f, 0.0f}, {0.0f, 1000.0f, 0.0f}, {0.0f, 1.0f, 0.0f});

                // z axis
                g_DebugManager->AddLine(rootNode, {0.f, 0.0f, -1000.0f}, {0.0f, 0.0f, 1000.0f}, {0.0f, 0.0f, 1.0f});
            }

            // Reset Camera
            m_EditorCamera.SetViewportSize(g_App->GetWindow().GetWidth(), g_App->GetWindow().GetHeight());
            m_EditorCamera.SetDistance(50.0f);
        }

        void OnUpdate()
        {
            m_EditorCamera.OnUpdate();

            g_GraphicsManager->SetViewProj(m_EditorCamera.GetView(), m_EditorCamera.GetProjection());

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

        void OnEvent(Event &event)
        {
            m_EditorCamera.OnEvent(event);
        }

    private:
        EditorCamera m_EditorCamera;

    };

    class EditorCameraGameLogic : public GameLogic
    {
    public:
        int Initialize()
        {
            PushLayer(new EditorCameraTestLayer());

            return 0;
        }
    };
}

namespace VKT {

    App             *g_App = new App();
    FileSystem      *g_FileSystem = new FileSystem();
    SceneManager    *g_SceneManager = new SceneManager();
    GraphicsManager *g_GraphicsManager = new GraphicsManager();
    InputManager    *g_InputManager = new InputManager();
    PhysicsManager  *g_PhysicsManager = new PhysicsManager();
    GameLogic       *g_GameLogic = new EditorCameraGameLogic();
    DebugManager    *g_DebugManager = new DebugManager();
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
    g_DebugManager->Initialize();

    while (g_App->IsRunning())
    {
        if (g_App->IsMinimized())
            continue;

        g_FileSystem->Tick();
        g_InputManager->Tick();
        g_SceneManager->Tick();
        g_PhysicsManager->Tick();
        g_GraphicsManager->Tick();
        g_GameLogic->Tick();
        g_DebugManager->Tick();
    }

    g_DebugManager->ShutDown();
    g_GameLogic->ShutDown();
    g_PhysicsManager->ShutDown();
    g_InputManager->ShutDown();
    g_GraphicsManager->ShutDown();
    g_SceneManager->ShutDown();
    g_FileSystem->ShutDown();
    g_App->ShutDown();

    return 0;
}