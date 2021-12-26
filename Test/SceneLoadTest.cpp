#include <Vkita.h>

#include <Core/FileSystem.h>
#include <Core/SceneManager.h>
#include <Core/GraphicsManager.h>

namespace VKT {
    Application     *g_App = new Application();
    FileSystem      *g_FileSystem = new FileSystem();
    SceneManager    *g_SceneManager = new SceneManager();
    GraphicsManager *g_GraphicsManager = new GraphicsManager();
}

using namespace VKT;

int main(int, char**)
{
    Log::Init();

    g_App->Initialize();
    g_FileSystem->Initialize();
    g_SceneManager->Initialize();
    g_GraphicsManager->Initialize();

    g_SceneManager->LoadScene(g_FileSystem->Append(g_FileSystem->GetRoot(), "Resource/Scenes/nanosuit/nanosuit.obj"));

    return 0;
}