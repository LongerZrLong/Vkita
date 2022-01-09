#include "SceneNode.h"

namespace VKT {

    glm::mat4 SceneNode::GetLocalToWorldMatrix() const
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

    void SceneNode::LinkRigidBody(void *rigidBody)
    {
        m_RigidBody = rigidBody;
    }

    void *SceneNode::UnlinkRigidBody()
    {
        void *rigidBody = m_RigidBody;
        m_RigidBody = nullptr;

        return rigidBody;
    }
}