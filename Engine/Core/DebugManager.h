#pragma once

#include <vector>
#include <unordered_map>

#include "Interface/IRuntimeModule.h"

#include "Scene/SceneNode.h"

#include "Rendering/Vertex.h"

namespace VKT {

    struct DebugVertex;

    class DebugManager : public IRuntimeModule
    {
    public:
        int Initialize() override;
        void ShutDown() override;
        void Tick() override;

        bool IsDrawDebugInfo() const { return m_DrawDebugInfo; }
        bool IsDebugInfoChanged() const { return m_DirtyFlag; }
        void ToggleDebugInfo() { m_DrawDebugInfo = !m_DrawDebugInfo; m_DirtyFlag = true; }
        void NotifySceneIsRenderingQueued() { m_DirtyFlag = false; }

        const auto &GetVerticesMap() const { return m_VerticesMap; }
        const auto &GetIndicesMap() const { return m_IndicesMap; }

        void AddLine(SceneNode &node, const glm::vec3 &from, const glm::vec3 &to, const glm::vec3 &color);

    private:
        bool m_DrawDebugInfo = false;
        bool m_DirtyFlag = false;

        std::unordered_map<SceneNode*, std::vector<DebugVertex>> m_VerticesMap;
        std::unordered_map<SceneNode*, std::vector<uint32_t>> m_IndicesMap;

    };

    extern DebugManager *g_DebugManager;

    struct DebugVertex
    {
        glm::vec3 a_Position;
        glm::vec3 a_Color;

        bool operator==(const DebugVertex &other) const
        {
            return
                a_Position == other.a_Position &&
                a_Color == other.a_Color;
        }

        static std::vector<VkVertexInputBindingDescription> GetBindingDescription()
        {
            return
                {
                    VKT::Vulkan::Initializers::vertexInputBindingDescription(0, sizeof(DebugVertex), VK_VERTEX_INPUT_RATE_VERTEX)
                };
        }

        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
        {
            return
                {
                    VKT::Vulkan::Initializers::vertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(DebugVertex, a_Position)),
                    VKT::Vulkan::Initializers::vertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(DebugVertex, a_Color)),
                };
        }
    };
}