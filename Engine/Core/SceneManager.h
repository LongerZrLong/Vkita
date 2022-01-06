#pragma once

#include "Core/Base.h"

#include "Interface/IRuntimeModule.h"

#include "Scene/Scene.h"

namespace VKT {

    class SceneManager : public IRuntimeModule
    {
    public:
        virtual ~SceneManager() = default;

        int Initialize() override;
        void ShutDown() override;

        void Tick() override;

        int LoadScene(const std::string &sceneFilePath);
        int ReloadScene();

        bool IsSceneChanged();
        Scene &GetScene();

        void NotifySceneIsRenderingQueued();

    private:
        Ref<Scene> m_Scene;
        bool m_DirtyFlag = false;

        std::string m_SceneFilePath;

    };

    extern SceneManager *g_SceneManager;

}