#include "SceneManager.h"

#include <string>

#include "Parser/AssimpParser.h"

namespace VKT {

    int SceneManager::Initialize()
    {
        m_Scene = CreateScope<Scene>();
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

        parser.ParseScene(m_Scene, sceneFilePath);

        if (!m_Scene)
            return -1;

        m_DirtyFlag = true;

        return 0;
    }

    bool SceneManager::IsSceneChanged()
    {
        return m_DirtyFlag;
    }

    Scene &SceneManager::GetScene()
    {
        return *m_Scene;
    }

    void SceneManager::NotifySceneIsRenderingQueued()
    {
        m_DirtyFlag = false;
    }

}