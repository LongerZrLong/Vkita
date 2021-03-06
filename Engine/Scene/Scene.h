#pragma once

#include <vector>
#include <list>
#include <unordered_map>
#include <string>

#include "Mesh.h"
#include "Material.h"
#include "Light.h"

#include "SceneNode.h"

#include "Core/Base.h"
#include "Core/Image.h"

#include "Rendering/Vertex.h"

namespace VKT {

    class Scene
    {
    public:
        std::unordered_map<std::string, Scope<Image>>       m_Textures;
        std::vector<Material>                               m_Materials;
        std::vector<Light>                                  m_Lights;

        std::vector<SceneNode>                              m_SceneNodes;   // Usually this is one dummy root node

        std::vector<Rendering::Vertex>                      m_Vertices;
        std::vector<uint32_t>                               m_Indices;

    };
}