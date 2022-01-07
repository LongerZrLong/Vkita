#include <Vkita.h>

#include <Core/FileSystem.h>
#include <Core/SceneManager.h>
#include <Core/GraphicsManager.h>
#include <Core/InputManager.h>
#include <Core/PhysicsManager.h>

namespace VKT {

    class PhysicsTestLayer : public Layer
    {
    public:
        void OnAttach()
        {
            g_SceneManager->LoadScene(g_FileSystem->Append(g_FileSystem->GetRoot(), "Resource/Scenes/basic_physics/basic_physics.gltf"));
        }

        void OnUpdate()
        {
            if (g_InputManager->IsKeyPressed(Key::R))
            {
                g_PhysicsManager->ClearRigidBodies();
                g_SceneManager->ReloadScene();

                // Manually set rigid bodies
                {
                    auto &scene = g_SceneManager->GetScene();

                    // Plane
                    scene.m_SceneNodes[0].m_Children[0].m_CollisionType = CollisionType::Sphere;

                    // Sphere
                    scene.m_SceneNodes[0].m_Children[1].m_CollisionType = CollisionType::Box;

                    // Sphere
                    scene.m_SceneNodes[0].m_Children[2].m_CollisionType = CollisionType::Sphere;
                }
            }
        }
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

    while (g_App->IsRunning())
    {
        g_App->Tick();
        g_FileSystem->Tick();
        g_InputManager->Tick();
        g_SceneManager->Tick();
        g_PhysicsManager->Tick();
        g_GraphicsManager->Tick();
        g_GameLogic->Tick();
    }

    g_GameLogic->Tick();
    g_PhysicsManager->ShutDown();
    g_InputManager->ShutDown();
    g_GraphicsManager->ShutDown();
    g_SceneManager->ShutDown();
    g_FileSystem->ShutDown();
    g_App->ShutDown();

    return 0;
}