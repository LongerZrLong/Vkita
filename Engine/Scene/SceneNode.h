#pragma once

#include <vector>

#include "Transform.h"
#include "Mesh.h"

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

    public:
        glm::mat4 GetLocalToWorldMatrix() const
        {
            Transform result;

            auto matrix = glm::identity<glm::mat4>();

            const SceneNode *ptr = this;

            while (ptr)
            {
                matrix = ptr->m_Transform.GetMatrix() * matrix;
                ptr = ptr->m_Parent;
            }

            return matrix;
        }

        void LinkRigidBody(void *rigidBody)
        {
            m_RigidBody = rigidBody;
        }

        void *UnlinkRigidBody()
        {
            void *rigidBody = m_RigidBody;
            m_RigidBody = nullptr;

            return rigidBody;
        }
    };
}
