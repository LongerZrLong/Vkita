#include "PhysicsManager.h"

#include "SceneManager.h"

namespace VKT {

    int PhysicsManager::Initialize()
    {
        // Build the broadphase
        m_btBroadphase = new btDbvtBroadphase();

        // Set up the collision configuration and dispatcher
        m_btCollisionConfiguration = new btDefaultCollisionConfiguration();
        m_btDispatcher = new btCollisionDispatcher(m_btCollisionConfiguration);

        // The actual physics solver
        m_btSolver = new btSequentialImpulseConstraintSolver;

        return 0;
    }

    void PhysicsManager::ShutDown()
    {
        // Clean up
        ClearRigidBodies();

        delete m_btSolver;
        delete m_btDispatcher;
        delete m_btCollisionConfiguration;
        delete m_btBroadphase;
    }

    void PhysicsManager::Tick()
    {
        if (g_SceneManager->IsSceneChanged())
        {
            g_PhysicsManager->ClearRigidBodies();
            g_PhysicsManager->CreateRigidBodies();
        }

        m_btDynamicsWorld->stepSimulation(1.0f / 60.0f, 10);
    }

    void PhysicsManager::CreateRigidBody(SceneNode &node)
    {
        btRigidBody *rigidBody = nullptr;

        switch (node.m_CollisionType)
        {
            case CollisionType::Sphere:
            {
                btCollisionShape *sphere = new btSphereShape(1.0f);
                m_btCollisionShapes.push_back(sphere);

                glm::mat4 matrix = node.GetLocalToWorldMatrix();

                auto *motionState = new btDefaultMotionState(
                    btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f),
                                btVector3(matrix[3][0], matrix[3][1], matrix[3][2])));

                btScalar mass = 1.0f;
                btVector3 fallInertia(0.0f, 0.0f, 0.0f);
                sphere->calculateLocalInertia(mass, fallInertia);

                btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, sphere, fallInertia);
                rigidBody = new btRigidBody(rigidBodyCI);

                m_btDynamicsWorld->addRigidBody(rigidBody);

                break;
            }
            case CollisionType::Box:
            {
                btCollisionShape *box = new btBoxShape(btVector3(5.0f, 0.01f, 5.0f));
                m_btCollisionShapes.push_back(box);

                glm::mat4 matrix = node.GetLocalToWorldMatrix();

                auto *motionState = new btDefaultMotionState(
                    btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f),
                                btVector3(matrix[3][0], matrix[3][1], matrix[3][2])));

                btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0.0f, motionState, box, btVector3(0.0f, 0.0f, 0.0f));
                rigidBody = new btRigidBody(rigidBodyCI);

                m_btDynamicsWorld->addRigidBody(rigidBody);

                break;
            }
            case CollisionType::Plane:
            {
                btCollisionShape *plane = new btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), 0);
                m_btCollisionShapes.push_back(plane);

                glm::mat4 matrix = node.GetLocalToWorldMatrix();

                auto *motionState = new btDefaultMotionState(
                    btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f),
                                btVector3(matrix[3][0], matrix[3][1], matrix[3][2])));

                btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0.0f, motionState, plane, btVector3(0.0f, 0.0f, 0.0f));
                rigidBody = new btRigidBody(rigidBodyCI);

                m_btDynamicsWorld->addRigidBody(rigidBody);

                break;
            }

            default:
                break;
        }

        node.LinkRigidBody(rigidBody);
    }

    void PhysicsManager::DFSCreateRigidBody(SceneNode &node)
    {
        CreateRigidBody(node);

        for (auto &child : node.m_Children)
        {
            DFSCreateRigidBody(child);
        }
    }

    void PhysicsManager::DFSDeleteRigidBody(SceneNode &node)
    {
        DeleteRigidBody(node);

        for (auto &child : node.m_Children)
        {
            DFSDeleteRigidBody(child);
        }
    }

    void PhysicsManager::DeleteRigidBody(SceneNode &node)
    {
        auto *rigidBody = reinterpret_cast<btRigidBody*>(node.UnlinkRigidBody());
        if (rigidBody)
        {
            delete rigidBody->getMotionState();
            delete rigidBody;
        }
    }

    int PhysicsManager::CreateRigidBodies()
    {
        // The world
        m_btDynamicsWorld = new btDiscreteDynamicsWorld(m_btDispatcher, m_btBroadphase, m_btSolver, m_btCollisionConfiguration);
        m_btDynamicsWorld->setGravity(btVector3(0.0f, -9.8f, 0.0f));

        auto &scene = g_SceneManager->GetScene();

        for (auto &node : scene.m_SceneNodes)
        {
            DFSCreateRigidBody(node);
        }

        return 0;
    }

    void PhysicsManager::ClearRigidBodies()
    {
        auto &scene = g_SceneManager->GetScene();

        for (auto &node : scene.m_SceneNodes)
        {
            DFSDeleteRigidBody(node);
        }

        for (auto shape : m_btCollisionShapes)
        {
            delete shape;
        }

        m_btCollisionShapes.clear();

        delete m_btDynamicsWorld;
    }

    glm::mat4 PhysicsManager::GetRigidBodyTransform(void *rigidBody)
    {
        btTransform trans;
        reinterpret_cast<btRigidBody*>(rigidBody)->getMotionState()->getWorldTransform(trans);

        auto basis = trans.getBasis();      // basis is row major 3x3 matrix
        auto origin = trans.getOrigin();

        auto result = glm::mat4(1.0f);

        result[0][0] = basis[0][0];
        result[1][0] = basis[0][1];
        result[2][0] = basis[0][2];
        result[0][1] = basis[1][0];
        result[1][1] = basis[1][1];
        result[2][1] = basis[1][2];
        result[0][2] = basis[2][0];
        result[1][2] = basis[2][1];
        result[2][2] = basis[2][2];

        result[3][0] = origin.getX();
        result[3][1] = origin.getY();
        result[3][2] = origin.getZ();

        return result;
    }
}
