#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Scene/Scene.h"

class aiScene;

namespace VKT {

    class AssimpParser
    {
    public:
        AssimpParser() = default;
        ~AssimpParser() = default;

        void ParseScene(Scope<Scene> &scene, const std::string &path);

    private:
        void ProcessMaterials(Scene*);
        void ProcessTextures(Scene*);
        void ProcessNode(aiNode*, Scene*, SceneNode*);

    private:
        const aiScene *m_AiScene;

        std::string m_SceneRootDir;

    };
}