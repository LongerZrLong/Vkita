#include <Core/FileSystem.h>
#include <Core/SceneManager.h>

namespace VKT {
    FileSystem      *g_FileSystem = new FileSystem();
    SceneManager    *g_SceneManager = new SceneManager();
}

using namespace VKT;

int main(int, char**)
{
    g_FileSystem->Initialize();
    g_SceneManager->Initialize();

    g_SceneManager->LoadScene(g_FileSystem->Append(g_FileSystem->GetRoot(), "Resource/Scenes/nanosuit/nanosuit.obj"));

    return 0;
}