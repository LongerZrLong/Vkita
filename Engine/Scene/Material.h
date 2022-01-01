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

        glm::vec4 m_DiffuseColor = glm::vec4(1.0f);
        glm::vec4 m_SpecularColor = glm::vec4(1.0f);

        float m_Shininess = 32.0f;

        std::string m_DiffuseTextureName;
        std::string m_SpecularTextureName;

        std::string m_NormalTextureName;

    };

}