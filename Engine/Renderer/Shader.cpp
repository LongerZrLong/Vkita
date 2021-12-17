#include "Shader.h"

#include "Renderer.h"
#include "GraphicsContext.h"

#include "Vulkan/ShaderModule.h"

namespace VKT {

    Shader::Shader(const std::string &filename)
    {
        m_ShaderModule = CreateScope<Vulkan::ShaderModule>(Renderer::GetGraphicsContext().GetDevice(), filename);
    }

    Shader::Shader(const std::vector<char> &code)
    {
        m_ShaderModule = CreateScope<Vulkan::ShaderModule>(Renderer::GetGraphicsContext().GetDevice(), code);
    }

    Shader::~Shader()
    {
    }
}