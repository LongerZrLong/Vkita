#include "SceneManager.h"

#include <string>

#include "FileSystem.h"

#include "Parser/AssimpParser.h"

namespace VKT {

    int SceneManager::Initialize()
    {
        m_Scene = CreateRef<Scene>();
        return 0;
    }

    void SceneManager::ShutDown()
    {
    }

    void SceneManager::Tick()
    {
    }

    int SceneManager::LoadScene(const std::string &sceneFilePath)
    {
        auto parser = AssimpParser();

        m_Scene = parser.ParseScene(sceneFilePath);

        if (!m_Scene)
            return -1;

        return 0;
    }

    bool SceneManager::IsSceneChanged()
    {
        return m_DirtyFlag;
    }

    const Scene &SceneManager::GetScene()
    {
        m_DirtyFlag = false;
        return *m_Scene;
    }

}