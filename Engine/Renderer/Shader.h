#pragma once

#include "Common/Base.h"

namespace VKT {

    namespace Vulkan {
        class ShaderModule;
    }

    class GraphicsPipeline;

    class Shader
    {
    public:
        NON_COPIABLE(Shader);

        explicit Shader(const std::string &filename);
        explicit Shader(const std::vector<char> &code);
        Shader() = delete;
        ~Shader();

        const Vulkan::ShaderModule &GetShaderModule() const { return *m_ShaderModule; }

    private:
        Scope<Vulkan::ShaderModule> m_ShaderModule;

    };
}