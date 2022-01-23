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

            // Manually set rigid bodies
            {
                auto &scene = g_SceneManager->GetScene();

                auto root = scene.m_SceneNodes.front();
                auto it = scene.m_SceneNodes.front().m_Children.begin();

                // Sphere
                it->m_CollisionType = CollisionType::Sphere;

                // Plane
                it++;
                it->m_CollisionType = CollisionType::Box;

                // Sphere
                it++;
                it->m_CollisionType = CollisionType::Sphere;
            }

            // Manually set debug info
            {
                SceneNode &rootNode = g_SceneManager->GetScene().m_SceneNodes.front();

                // x axis
                g_DebugManager->AddLine(rootNode, {-1000.f, 0.0f, 0.0f}, {1000.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});

                // y axis
                g_DebugManager->AddLine(rootNode, {0.f, -1000.0f, 0.0f}, {0.0f, 1000.0f, 0.0f}, {0.0f, 1.0f, 0.0f});

                // z axis
                g_DebugManager->AddLine(rootNode, {0.f, 0.0f, -1000.0f}, {0.0f, 0.0f, 1000.0f}, {0.0f, 0.0f, 1.0f});

                AddBoundingBox(rootNode);
            }

            Eye = glm::vec3(-10.0f, 10.0f, 10.0f);
            Center = glm::vec3(0.0f, 0.0f, 0.0f);
            Up = glm::vec3(0.0f, 1.0f, 0.0f);

            View = glm::lookAt(Eye, Center, Up);
            Proj = glm::perspective(glm::radians(60.0f),
                                    (float)g_App->GetWindow().GetWidth() / (float)g_App->GetWindow().GetHeight(),
                                    0.1f, 1000.0f);
        }

        void OnUpdate()
        {
            g_GraphicsManager->SetViewProj(View, Proj);

            static bool prevReleased = true;

            if (g_InputManager->IsKeyPressed(Key::R))
            {
                if (prevReleased)
                    g_GraphicsManager->ResetScene();

                prevReleased = false;
                return;
            }

            if (g_InputManager->IsKeyPressed(Key::D))
            {
                if (prevReleased)
                    g_DebugManager->ToggleDebugInfo();

                prevReleased = false;
                return;
            }

            prevReleased = true;
        }


        void AddBoundingBox(SceneNode &node)
        {
            BoundingBox bbox = node.m_Mesh.GetBoundingBox();
            g_DebugManager->AddBox(node, bbox.Centroid - bbox.Extent, bbox.Centroid + bbox.Extent);

            for (auto &child : node.m_Children)
            {
                AddBoundingBox(child);
            }
        }

    private:
        glm::mat4 View;
        glm::mat4 Proj;

        glm::vec3 Eye;
        glm::vec3 Center;
        glm::vec3 Up;
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