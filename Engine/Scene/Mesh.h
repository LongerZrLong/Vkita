#pragma once

#include "Material.h"

#include "Renderer/Buffer.h"

namespace VKT {

    struct Primitive
    {
        uint32_t FirstIndex;
        uint32_t IndexCount;
        uint32_t MaterialIndex;
    };

    struct Mesh
    {
        std::vector<Primitive> m_Primitives;
    };
}