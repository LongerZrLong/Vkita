#include "Mesh.h"

#include "Core/SceneManager.h"

namespace VKT {

    BoundingBox Mesh::GetBoundingBox() const
    {
        auto &scene = g_SceneManager->GetScene();
        
        glm::vec3 bbmin(std::numeric_limits<float>::max());
        glm::vec3 bbmax(std::numeric_limits<float>::min());

        for (const auto &primitive : m_Primitives)
        {
            for (size_t i = 0; i < primitive.IndexCount; i++)
            {
                glm::vec3 pos = scene.m_Vertices[scene.m_Indices[primitive.FirstIndex + i]].a_Position;
                bbmin.x = std::min(bbmin.x, pos.x);
                bbmin.y = std::min(bbmin.y, pos.y);
                bbmin.z = std::min(bbmin.z, pos.z);

                bbmax.x = std::max(bbmax.x, pos.x);
                bbmax.y = std::max(bbmax.y, pos.y);
                bbmax.z = std::max(bbmax.z, pos.z);
            }
        }

        return {(bbmax + bbmin) * 0.5f, (bbmax - bbmin) * 0.5f};
    }
}