#include <Core/FileSystem.h>
#include <Core/SceneManager.h>

namespace VKT {
    FileSystem      *g_FileSystem = new FileSystem();
    SceneManager    *g_SceneManager = new SceneManager();
}

using namespace VKT;

void PrintNodeName(SceneNode *node, int level)
{
    std::cout << std::string(level * 4, ' ') << node->m_Name << std::endl;

    for (auto &child : node->m_Children)
    {
        PrintNodeName(&child, level + 1);
    }
}

int main(int, char**)
{
    g_FileSystem->Initialize();
    g_SceneManager->Initialize();

    g_SceneManager->LoadScene(g_FileSystem->Append(g_FileSystem->GetRoot(), "Resource/Scenes/lights/lights.gltf"));

    auto &scene = g_SceneManager->GetScene();
    for (auto &node : scene.m_SceneNodes)
    {
        PrintNodeName(&node, 0);
    }

    return 0;
}