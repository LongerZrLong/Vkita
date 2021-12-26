#pragma once

#include <vector>

#include "Transform.h"
#include "Mesh.h"

#include "Core/Base.h"

namespace VKT {

    class SceneNode
    {
    public:
        SceneNode() : m_Transform(*this) {};

        std::string m_Name;
        SceneNode *m_Parent;
        std::vector<Scope<SceneNode>> m_Children;

        Mesh m_Mesh;

        Transform m_Transform;
        bool m_Visible = true;
    };
}
