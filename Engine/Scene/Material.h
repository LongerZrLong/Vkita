#pragma once

#include <string>

#include "Math/Glm.h"

#include "Core/Base.h"

#include "Vulkan/DescriptorSet.h"
#include "Vulkan/Pipeline.h"

namespace VKT {

    class Material
    {
    public:
        std::string m_Name;

        struct Parameter
        {
            alignas(16) glm::vec4 DiffuseColor = glm::vec4(1.0f);
            alignas(16) glm::vec4 SpecularColor = glm::vec4(1.0f);

            alignas(4) float Shininess = 32.0f;

        } m_Parameter;

        std::string m_DiffuseTextureName;
        std::string m_SpecularTextureName;

        std::string m_NormalTextureName;

    };

}