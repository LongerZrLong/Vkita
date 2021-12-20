#pragma once

#include "Shader.h"
#include "DescriptorSetManager.h"

#include "Common/Base.h"

#include "Vulkan/Pipeline.h"
#include "Vulkan/PipelineLayout.h"

namespace VKT {

    class GraphicsPipeline
    {
    public:
        NON_COPIABLE(GraphicsPipeline);

        GraphicsPipeline(const Ref<Shader> &vertShader,
                         const Ref<Shader> &fragShader,
                         const Ref<DescriptorSetManager> &descriptorSetManager);

        ~GraphicsPipeline() = default;

        const Vulkan::Pipeline &GetPipeline() const { return *m_Pipeline; }
        const Vulkan::PipelineLayout &GetPipelineLayout() const { return *m_PipelineLayout; }

    private:
        Scope<Vulkan::Pipeline> m_Pipeline;
        Scope<Vulkan::PipelineLayout> m_PipelineLayout;

        const Ref<Shader> &m_VertShader;
        const Ref<Shader> &m_FragShader;
        const Ref<DescriptorSetManager> &m_DescriptorSetManager;

    };
}