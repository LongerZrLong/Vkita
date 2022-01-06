#pragma once

#include <vector>

#include <btBulletDynamicsCommon.h>

#include "Interface/IRuntimeModule.h"

#include "Scene/SceneNode.h"

#include "Math/Glm.h"

namespace VKT {

    class PhysicsManager : public IRuntimeModule
    {
    public:
        PhysicsManager() = default;
        ~PhysicsManager() override = default;

        int Initialize() override;
        void ShutDown() override;

        void Tick() override;

        virtual void CreateRigidBody(SceneNode &node);
        virtual void DeleteRigidBody(SceneNode &node);

        virtual int CreateRigidBodies();
        virtual void ClearRigidBodies();

        glm::mat4 GetRigidBodyTransform(void *rigidBody);

    private:
        void DFSCreateRigidBody(SceneNode &node);
        void DFSDeleteRigidBody(SceneNode &node);

    private:
        btBroadphaseInterface                   *m_btBroadphase;
        btDefaultCollisionConfiguration         *m_btCollisionConfiguration;
        btCollisionDispatcher                   *m_btDispatcher;
        btSequentialImpulseConstraintSolver     *m_btSolver;
        btDiscreteDynamicsWorld                 *m_btDynamicsWorld;

        std::vector<btCollisionShape*>          m_btCollisionShapes;

    };

    extern PhysicsManager *g_PhysicsManager;
}