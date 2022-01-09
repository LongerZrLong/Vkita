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

        m_IndicesMap[&node].push_back(curSize);
        m_IndicesMap[&node].push_back(curSize + 1);
    }

}