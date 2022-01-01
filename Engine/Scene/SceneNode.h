#pragma once

#include <vector>

#include "Transform.h"
#include "Mesh.h"

#include "Core/Base.h"

namespace VKT {

    class SceneNode
    {
    public:
        std::string m_Name;
        SceneNode *m_Parent;
        std::vector<SceneNode> m_Children;

        Mesh m_Mesh;

        Transform m_Transform;
        bool m_Visible = true;
    };
}
