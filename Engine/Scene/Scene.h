#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "Mesh.h"
#include "Material.h"
#include "SceneNode.h"

#include "Core/Base.h"

#include "Renderer/Texture2D.h"
#include "Renderer/Vertex.h"

namespace VKT {

    class Scene
    {
    public:
        std::unordered_map<std::string, Scope<Image>>       m_Textures;
        std::vector<Material>                               m_Materials;
        std::vector<SceneNode>                              m_SceneNodes;

        std::vector<Vertex>     m_Vertices;
        std::vector<uint32_t>   m_Indices;

    };
}