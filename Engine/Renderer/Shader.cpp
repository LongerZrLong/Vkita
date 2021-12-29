#include "Shader.h"

#include "Core/GraphicsManager.h"

#include "Vulkan/ShaderModule.h"

namespace VKT {

    Shader::Shader(const std::string &path)
    {
        m_ShaderModule = CreateScope<Vulkan::ShaderModule>(g_GraphicsManager->GetDevice(), path);
    }

    Shader::Shader(const std::vector<char> &code)
    {
        m_ShaderModule = CreateScope<Vulkan::ShaderModule>(g_GraphicsManager->GetDevice(), code);
    }

    Shader::~Shader()
    {
    }
}