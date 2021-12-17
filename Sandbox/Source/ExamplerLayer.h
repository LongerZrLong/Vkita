#pragma once

#include <Vkita.h>

using namespace VKT;

class ExampleLayer : public Layer
{
public:
    ExampleLayer();
    ~ExampleLayer() = default;

    void OnUpdate(VKT::Timestep ts) override;

private:
    Ref<GraphicsPipeline> m_GraphicsPipeline;

    Ref<Shader> m_VertShader;
    Ref<Shader> m_FragShader;

    Ref<VertexBuffer> m_VertexBuffer;
    Ref<IndexBuffer> m_IndexBuffer;

    Ref<DescriptorSetManager> m_DescriptorSetManager;
    Ref<UniformBuffer> m_UniformBuffer;
    Ref<Texture2D> m_CheckerBoardTex;

    float m_ElapsedTime = 0.0f;
    float m_CurrentTimestep = 0.0f;
};