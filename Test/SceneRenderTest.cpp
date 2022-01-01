#include <Vkita.h>

#include <Core/FileSystem.h>
#include <Core/SceneManager.h>
#include <Core/GraphicsManager.h>
#include <Core/InputManager.h>

namespace VKT {

    Application     *g_App = new Application();
    FileSystem      *g_FileSystem = new FileSystem();
    SceneManager    *g_SceneManager = new SceneManager();
    GraphicsManager *g_GraphicsManager = new GraphicsManager();
    InputManager    *g_InputManager = new InputManager();

}

using namespace VKT;

int main()
{
    g_App->Initialize();
    g_InputManager->Initialize();
    g_FileSystem->Initialize();
    g_GraphicsManager->Initialize();
    g_SceneManager->Initialize();

    g_SceneManager->LoadScene(g_FileSystem->Append(g_FileSystem->GetRoot(), "Resource/Scenes/nanosuit/nanosuit.obj"));

    while (g_App->IsRunning())
    {
        g_App->Tick();
        g_FileSystem->Tick();
        g_InputManager->Tick();
        g_GraphicsManager->Tick();
        g_SceneManager->Tick();
    }

    g_InputManager->ShutDown();
    g_GraphicsManager->ShutDown();
    g_SceneManager->ShutDown();
    g_FileSystem->ShutDown();
    g_App->ShutDown();

    return 0;
}