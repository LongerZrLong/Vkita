#pragma once

#include "Math/Glm.h"
#include "Vulkan/Initializers.h"

namespace VKT {

    struct Vertex
    {
        glm::vec3 a_Position;
        glm::vec3 a_Normal;
        glm::vec3 a_Tangent;
        glm::vec3 a_Bitangent;
        glm::vec2 a_TexCoord;

        bool operator==(const Vertex &other) const
        {
            return
            a_Position == other.a_Position &&
            a_Normal == other.a_Normal &&
            a_Tangent == other.a_Tangent &&
            a_Bitangent == other.a_Bitangent &&
            a_TexCoord == other.a_TexCoord;
        }

        static std::vector<VkVertexInputBindingDescription> GetBindingDescription()
        {
            return
            {
                Vulkan::Initializers::vertexInputBindingDescription(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX)
            };
        }

        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
        {
            return
            {
                Vulkan::Initializers::vertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, a_Position)),
                Vulkan::Initializers::vertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, a_Normal)),
                Vulkan::Initializers::vertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, a_Tangent)),
                Vulkan::Initializers::vertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, a_Bitangent)),
                Vulkan::Initializers::vertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, a_TexCoord)),
            };
        }
    };
}
