#pragma once

#include <vector>
#include <unordered_map>

#include <vulkan/vulkan.h>

#include "Core/Base.h"

#include "Interface/IRuntimeModule.h"

#include "Vulkan/DescriptorPool.h"
#include "Vulkan/DescriptorSetLayout.h"
#include "Vulkan/DescriptorSet.h"
#include "Vulkan/ShaderModule.h"
#include "Vulkan/PipelineLayout.h"
#include "Vulkan/Pipeline.h"

#include "Rendering/Context.h"
#include "Rendering/Buffer.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/Texture2D.h"

#include "Scene/SceneNode.h"

namespace VKT {

    class GraphicsManager : public IRuntimeModule
    {
    public:
        GraphicsManager() = default;
        ~GraphicsManager() override = default;

        int Initialize() override;
        void ShutDown() override;

        void Tick() override;

        const Rendering::Context &GetContext() const { return *m_Ctx; }

    private:
        void InitializeGeometries();
        void PreparePipeline();
        void BuildCommandBuffers();
        void DrawNode(VkCommandBuffer vkCommandBuffer, const SceneNode &node);

    private:
        bool BeginFrame();
        void EndFrame();

        void Present();

    private:
        Scope<Rendering::Context> m_Ctx;

        // TODO: Delete Testing Code
        Ref<Vulkan::DescriptorPool> m_DescriptorPool;

        Ref<Vulkan::PipelineLayout> m_PipelineLayout;
        Ref<Vulkan::Pipeline> m_GraphicsPipeline;

        Ref<Vulkan::ShaderModule> m_VertShader;
        Ref<Vulkan::ShaderModule> m_FragShader;

        Ref<Rendering::VertexBuffer> m_VertexBuffer;
        Ref<Rendering::IndexBuffer> m_IndexBuffer;

        struct ShaderData
        {
            Scope<Rendering::Buffer> buffer;
            struct Values
            {
                alignas(16) glm::mat4 View;
                alignas(16) glm::mat4 Proj;
            } values;
        } m_ShaderData;

        struct Camera
        {
            glm::vec3 Eye = {0.0f, 10.0f, 30.0f};
            glm::vec3 Center = {0.0f, 10.0f, 0.0f};
            glm::vec3 Up = {0.0f, 1.0f, 0.0f};
        } m_Camera;

        friend class InputManager;

        Scope<Vulkan::DescriptorSetLayout> m_MatricesDescSetLayout;
        Scope<Vulkan::DescriptorSetLayout> m_MaterialDescSetLayout;

        Scope<Vulkan::DescriptorSet> m_MatricesDescSet;

        std::vector<Scope<Vulkan::DescriptorSet>> m_MaterialDescSets;

        struct MaterialUBO
        {
            Scope<Rendering::Buffer> buffer;
            struct Values
            {
                alignas(4) bool HasDiffMap;
                alignas(4) bool HasSpecMap;
                alignas(16) glm::vec4 DiffColor;
                alignas(16) glm::vec4 SpecColor;
            } values;
        };
        std::vector<MaterialUBO> m_MaterialUniformBuffers;
        std::unordered_map<std::string, Scope<Rendering::Texture2D>> m_Textures;

        bool m_Prepared = false;
    };

    extern GraphicsManager *g_GraphicsManager;

}