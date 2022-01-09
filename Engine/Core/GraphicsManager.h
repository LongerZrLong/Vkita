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
        void DrawNode(VkCommandBuffer vkCommandBuffer, SceneNode &node);

        void SetupRuntimeModelMatrixUBOMap(SceneNode &node);
        void SetupRuntimeModelMatrixDescSetMap(SceneNode &node);
        void WriteRuntimeModelMatrixDescSetMap(SceneNode &node);

        void UpdateRuntimeNodeModelMatrix(SceneNode &node);

        void InitializeDebugInfo();
        void PrepareDebugPipeline();

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
            glm::vec3 Eye = {0.0f, 10.0f, 20.0f};
            glm::vec3 Center = {0.0f, 0.0f, 0.0f};
            glm::vec3 Up = {0.0f, 1.0f, 0.0f};
        } m_Camera;

        Scope<Vulkan::DescriptorSetLayout> m_MatricesDescSetLayout;
        Scope<Vulkan::DescriptorSetLayout> m_ModelMatrixSetLayout;
        Scope<Vulkan::DescriptorSetLayout> m_MaterialDescSetLayout;

        Scope<Vulkan::DescriptorSet> m_MatricesDescSet;

        struct ModelMatrixUBO
        {
            Scope<Rendering::Buffer> buffer;
            struct Values
            {
                alignas(16) glm::mat4 Model;
            } values;
        } ;

        std::unordered_map<SceneNode*, ModelMatrixUBO> m_RuntimeModelMatrixUBOMap;
        std::unordered_map<SceneNode*, Scope<Vulkan::DescriptorSet>> m_RuntimeModelMatrixDescSetMap;

        std::vector<Scope<Vulkan::DescriptorSet>> m_MaterialDescSets;

        struct MaterialUBO
        {
            Scope<Rendering::Buffer> buffer;
            struct Values
            {
                alignas(16) glm::vec4 DiffColor;
                alignas(16) glm::vec4 SpecColor;
            } values;
        };
        std::vector<MaterialUBO> m_MaterialUniformBuffers;
        std::unordered_map<std::string, Scope<Rendering::Texture2D>> m_Textures;

        // TODO: This is a Debug Info Demo, remove in the future
        struct DebugVertex
        {
            glm::vec3 a_Position;
            glm::vec3 a_Color;
        };

        std::vector<DebugVertex> m_DebugVertices;
        Scope<Rendering::Buffer> m_DebugVertBuffer;

        Ref<Vulkan::PipelineLayout> m_DebugPipelineLayout;
        Ref<Vulkan::Pipeline> m_DebugGraphicsPipeline;

        Ref<Vulkan::ShaderModule> m_DebugVertShader;
        Ref<Vulkan::ShaderModule> m_DebugFragShader;
    };

    extern GraphicsManager *g_GraphicsManager;

}