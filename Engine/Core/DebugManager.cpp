#include "DebugManager.h"

namespace VKT {

    int DebugManager::Initialize()
    {
        return 0;
    }

    void DebugManager::ShutDown()
    {
    }

    void DebugManager::Tick()
    {
    }

    void DebugManager::AddLine(SceneNode &node, const glm::vec3 &from, const glm::vec3 &to, const glm::vec3 &color)
    {
        size_t curSize = m_VerticesMap[&node].size();

        m_VerticesMap[&node].push_back({from, color});
        m_VerticesMap[&node].push_back({to, color});

        m_IndicesMap[&node].push_back(curSize + 0);
        m_IndicesMap[&node].push_back(curSize + 1);
    }

    void DebugManager::AddBox(SceneNode &node, const glm::vec3 &boxMin, const glm::vec3 &boxMax, const glm::vec3 &color)
    {
        size_t curSize = m_VerticesMap[&node].size();

        m_VerticesMap[&node].push_back({{boxMin.x, boxMin.y, boxMin.z}, color});    // 0
        m_VerticesMap[&node].push_back({{boxMax.x, boxMin.y, boxMin.z}, color});    // 1
        m_VerticesMap[&node].push_back({{boxMax.x, boxMin.y, boxMax.z}, color});    // 2
        m_VerticesMap[&node].push_back({{boxMin.x, boxMin.y, boxMax.z}, color});    // 3

        m_VerticesMap[&node].push_back({{boxMin.x, boxMax.y, boxMin.z}, color});    // 4
        m_VerticesMap[&node].push_back({{boxMax.x, boxMax.y, boxMin.z}, color});    // 5
        m_VerticesMap[&node].push_back({{boxMax.x, boxMax.y, boxMax.z}, color});    // 6
        m_VerticesMap[&node].push_back({{boxMin.x, boxMax.y, boxMax.z}, color});    // 7

        // bottom
        m_IndicesMap[&node].push_back(curSize + 0);
        m_IndicesMap[&node].push_back(curSize + 1);

        m_IndicesMap[&node].push_back(curSize + 1);
        m_IndicesMap[&node].push_back(curSize + 2);

        m_IndicesMap[&node].push_back(curSize + 2);
        m_IndicesMap[&node].push_back(curSize + 3);

        m_IndicesMap[&node].push_back(curSize + 3);
        m_IndicesMap[&node].push_back(curSize + 0);

        // top
        m_IndicesMap[&node].push_back(curSize + 4);
        m_IndicesMap[&node].push_back(curSize + 5);

        m_IndicesMap[&node].push_back(curSize + 5);
        m_IndicesMap[&node].push_back(curSize + 6);

        m_IndicesMap[&node].push_back(curSize + 6);
        m_IndicesMap[&node].push_back(curSize + 7);

        m_IndicesMap[&node].push_back(curSize + 7);
        m_IndicesMap[&node].push_back(curSize + 4);

        // side
        m_IndicesMap[&node].push_back(curSize + 0);
        m_IndicesMap[&node].push_back(curSize + 4);

        m_IndicesMap[&node].push_back(curSize + 1);
        m_IndicesMap[&node].push_back(curSize + 5);

        m_IndicesMap[&node].push_back(curSize + 2);
        m_IndicesMap[&node].push_back(curSize + 6);

        m_IndicesMap[&node].push_back(curSize + 3);
        m_IndicesMap[&node].push_back(curSize + 7);
    }

}