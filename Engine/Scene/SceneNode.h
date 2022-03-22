#pragma once

#include <vector>
#include <list>

#include "Transform.h"
#include "Mesh.h"
#include "Light.h"

#include "Core/Base.h"

namespace VKT {

    enum class CollisionType
    {
        None = 0, Sphere, Box, Plane,
    };

    class SceneNode
    {
    public:
        std::string m_Name;
        SceneNode *m_Parent;
        std::vector<SceneNode> m_Children;

        Mesh m_Mesh;

        Transform m_Transform;
        bool m_Visible = true;

        CollisionType m_CollisionType = CollisionType::None;
        void *m_RigidBody = nullptr;

        Light *m_Light = nullptr;

    public:
        glm::mat4 GetLocalToWorldMatrix() const;

        void LinkRigidBody(void *rigidBody);
        void *UnlinkRigidBody();

    };
}
