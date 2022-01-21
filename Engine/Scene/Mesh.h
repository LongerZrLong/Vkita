#pragma once

#include "Material.h"

namespace VKT {

    struct Primitive
    {
        uint32_t FirstIndex;
        uint32_t IndexCount;
        uint32_t MaterialIndex;
    };

    struct BoundingBox
    {
        glm::vec3 Centroid;
        glm::vec3 Extent;
    };

    struct Mesh
    {
        std::vector<Primitive> m_Primitives;

        BoundingBox GetBoundingBox() const;
    };
}