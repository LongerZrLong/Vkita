#pragma once

#include <array>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

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

        static VkVertexInputBindingDescription GetBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription = {};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 5> GetAttributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, a_Position);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, a_Normal);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, a_Tangent);

            attributeDescriptions[3].binding = 0;
            attributeDescriptions[3].location = 3;
            attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[3].offset = offsetof(Vertex, a_Bitangent);

            attributeDescriptions[4].binding = 0;
            attributeDescriptions[4].location = 4;
            attributeDescriptions[4].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[4].offset = offsetof(Vertex, a_TexCoord);

            return attributeDescriptions;
        }
    };
}
