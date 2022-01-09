#include <Vkita.h>

#include <Core/FileSystem.h>
#include <Core/SceneManager.h>
#include <Core/GraphicsManager.h>
#include <Core/InputManager.h>
#include <Core/PhysicsManager.h>
#include <Core/DebugManager.h>

namespace VKT {

    class PhysicsTestLayer : public Layer
    {
    public:
        void OnAttach()
        {
            g_SceneManager->LoadScene(g_FileSystem->Append(g_FileSystem->GetRoot(), "Resource/Scenes/basic_physics/basic_physics.gltf"));
            AddDebugInfo();
        }

        void OnUpdate()
        {
            if (g_InputManager->IsKeyPressed(Key::R))
            {
                m_ResetKeyPressed = true;
            }

            if (!g_InputManager->IsKeyPressed(Key::R) && m_ResetKeyPressed)
            {
                g_PhysicsManager->ClearRigidBodies();

                // TODO: Temporarily use reload scene. Implement reset scene in graphics manager in the future
                g_SceneManager->ReloadScene();

                // Manually set rigid bodies
                {
                    auto &scene = g_SceneManager->GetScene();

                    // Sphere
                    scene.m_SceneNodes[0].m_Children[0].m_CollisionType = CollisionType::Sphere;

                    // Plane
                    scene.m_SceneNodes[0].m_Children[1].m_CollisionType = CollisionType::Box;

                    // Move the plane up a little
                    scene.m_SceneNodes[0].m_Children[1].m_Transform.SetPosition({0.0f, 0.0f, 0.0f});

                    // Sphere
                    scene.m_SceneNodes[0].m_Children[2].m_CollisionType = CollisionType::Sphere;
                }

                AddDebugInfo();

                m_ResetKeyPressed = false;
            }

            if (g_InputManager->IsKeyPressed(Key::D))
            {
                m_DebugKeyPressed = true;
            }

            if (!g_InputManager->IsKeyPressed(Key::D) && m_DebugKeyPressed)
            {
                g_DebugManager->ToggleDebugInfo();
                m_DebugKeyPressed = false;
            }
        }

    private:
        void AddDebugInfo()
        {
            // Manually set debug info
            SceneNode &rootNode = g_SceneManager->GetScene().m_SceneNodes[0];

            // x axis
            g_DebugManager->AddLine(rootNode, {-1000.f, 0.0f, 0.0f}, {1000.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});

            // y axis
            g_DebugManager->AddLine(rootNode, {0.f, -1000.0f, 0.0f}, {0.0f, 1000.0f, 0.0f}, {0.0f, 1.0f, 0.0f});

            // z axis
            g_DebugManager->AddLine(rootNode, {0.f, 0.0f, -1000.0f}, {0.0f, 0.0f, 1000.0f}, {0.0f, 0.0f, 1.0f});
        }

        bool m_ResetKeyPressed = false;
        bool m_DebugKeyPressed = false;
    };

    class PhysicsTestGameLogic : public GameLogic
    {
    public:
        int Initialize()
        {
            PushLayer(new PhysicsTestLayer());

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
    GameLogic       *g_GameLogic = new PhysicsTestGameLogic();
    DebugManager    *g_DebugManager = new DebugManager();
}

using namespace VKT;

int main()
{
    g_App->Initialize();
    g_InputManager->Initialize();
    g_FileSystem->Initialize();
    g_GraphicsManager->Initialize();
    g_SceneManager->Initialize();
    g_PhysicsManager->Initialize();
    g_GameLogic->Initialize();
    g_DebugManager->Initialize();

    while (g_App->IsRunning())
    {
        g_App->Tick();
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