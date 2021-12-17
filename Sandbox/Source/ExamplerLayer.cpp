#include "ExamplerLayer.h"

#include <iostream>

ExampleLayer::ExampleLayer()
{
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, {0.2f, 0.3f, 0.9f}, {0.0, 0.0}},
        {{0.5f, -0.5f, 0.0f}, {0.2f, 0.3f, 0.9f}, {1.0, 0.0}},
        {{0.5f, 0.5f, 0.0f}, {0.2f, 0.3f, 0.9f}, {1.0, 1.0}},
        {{-0.5f, 0.5f, 0.0f}, {0.2f, 0.3f, 0.9f}, {0.0, 1.0}},
    };

    const std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0
    };

    m_VertexBuffer = CreateRef<VertexBuffer>(vertices);
    m_IndexBuffer = CreateRef<IndexBuffer>(indices);

    m_UniformBuffer = CreateRef<UniformBuffer>();

    UniformBufferObject ubo = {};
    ubo.Model = glm::mat4(1.0f);
    ubo.View = glm::mat4(1.0f);
    ubo.Proj = glm::mat4(1.0f);
    m_UniformBuffer->Update(ubo);

    m_CheckerBoardTex = CreateRef<Texture2D>("Resource/Textures/Checkerboard.png");

    m_VertShader = CreateRef<Shader>("Resource/Shaders/shader.vert");
    m_FragShader = CreateRef<Shader>("Resource/Shaders/shader.frag");

    std::vector<DescriptorBinding> descriptorBindings =
        {
            {0, 1, m_UniformBuffer.get(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
            {1, 1, m_CheckerBoardTex.get(), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
        };
    m_DescriptorSetManager = CreateRef<DescriptorSetManager>(descriptorBindings);

    m_GraphicsPipeline = CreateRef<GraphicsPipeline>(m_VertShader, m_FragShader, m_DescriptorSetManager);
}

void ExampleLayer::OnUpdate(VKT::Timestep ts)
{
    m_CurrentTimestep = ts;
    m_ElapsedTime += ts;

    Window &window = Application::Get().GetWindow();

    UniformBufferObject ubo{};
    ubo.Model = glm::rotate(glm::mat4(1.0f), m_ElapsedTime * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.View = glm::mat4(1.0f);
    ubo.Proj = glm::mat4(1.0f);

    m_UniformBuffer->Update(ubo);

    Renderer::Draw(m_GraphicsPipeline, m_VertexBuffer, m_IndexBuffer, m_DescriptorSetManager);
}
