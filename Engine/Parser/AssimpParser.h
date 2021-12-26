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

        Ref<Scene> ParseScene(const std::string &path);

    private:
        void ProcessMaterials(Ref<Scene>&);
        void ProcessTextures(Ref<Scene>&);
        void ProcessNode(aiNode*, Ref<Scene>&, SceneNode*);

    private:
        const aiScene *m_AiScene;

        std::string m_SceneRootDir;

    };
}