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

        void ResetScene();

        void SetViewProj(glm::mat4 View, glm::mat4 Projection);

        const Rendering::Context &GetContext() const { return *m_Ctx; }

    private:
        void InitializeGeometries();
        void PreparePipeline();
        void BuildCommandBuffers();
        void DrawNode(VkCommandBuffer vkCommandBuffer, SceneNode &node);

        void SetupRuntimePerBatchUboDict(SceneNode &node);
        void SetupRuntimePerBatchDescSetDict(SceneNode &node);
        void WriteRuntimePerBatchDescSetDict(SceneNode &node);

        void UpdateRuntimeNodeModelMatrix(SceneNode &node);
        void ResetRuntimeNodeModelMatrix(SceneNode &node);

        void CalculateLights();

        // Debug
        void InitializeDebugInfo();
        void PrepareDebugPipeline();
        void DrawNodeDebugInfo(VkCommandBuffer vkCommandBuffer, SceneNode &node);

    private:
        bool BeginFrame();
        void EndFrame();

        void Present();

    private:
        Scope<Rendering::Context> m_Ctx;

        Ref<Vulkan::DescriptorPool> m_DescriptorPool;

        Ref<Vulkan::PipelineLayout> m_PipelineLayout;
        Ref<Vulkan::Pipeline> m_GraphicsPipeline;

        Ref<Vulkan::ShaderModule> m_VertShader;
        Ref<Vulkan::ShaderModule> m_FragShader;

        Ref<Rendering::VertexBuffer> m_VertexBuffer;
        Ref<Rendering::IndexBuffer> m_IndexBuffer;

        // TODO: Light descriptor set layout should be within per frame descriptor set layout
        Scope<Vulkan::DescriptorSetLayout> m_PerFrameDescSetLayout;
        Scope<Vulkan::DescriptorSetLayout> m_LightDescSetLayout;
        Scope<Vulkan::DescriptorSetLayout> m_PerBatchDescSetLayout;
        Scope<Vulkan::DescriptorSetLayout> m_MaterialDescSetLayout;

        // PerFrame view projection matrices
        struct
        {
            struct
            {
                alignas(16) glm::mat4 View;
                alignas(16) glm::mat4 Proj;
            } Values;

            Scope<Rendering::Buffer> Ubo;
            Scope<Vulkan::DescriptorSet> DescSet;
        } m_ViewProj;

        // PerBatch model matrix
        std::unordered_map<SceneNode*, Scope<Rendering::Buffer>> m_RuntimePerBatchUboDict;
        std::unordered_map<SceneNode*, Scope<Vulkan::DescriptorSet>> m_RuntimePerBatchDescSetDict;

        // PerBatch PerMesh Material
        std::vector<Scope<Vulkan::DescriptorSet>> m_MaterialDescSets;
        std::vector<Scope<Rendering::Buffer>> m_MaterialUbos;
        std::unordered_map<std::string, Scope<Rendering::Texture2D>> m_TextureDict;

        // PerFrame Light
        Scope<Rendering::Buffer> m_LightUbo;
        Scope<Vulkan::DescriptorSet> m_LightDescSet;

        // Debug Info
        struct DebugPrimitive
        {
            size_t FirstIndex;
            size_t IndexCount;
        };
        std::unordered_map<SceneNode*, DebugPrimitive> m_DebugPrimitivesDict;

        Ref<Rendering::Buffer> m_DebugVertBuffer;
        Ref<Rendering::Buffer> m_DebugIndexBuffer;

        Ref<Vulkan::PipelineLayout> m_DebugPipelineLayout;
        Ref<Vulkan::Pipeline> m_DebugGraphicsPipeline;

        Ref<Vulkan::ShaderModule> m_DebugVertShader;
        Ref<Vulkan::ShaderModule> m_DebugFragShader;
    };

    extern GraphicsManager *g_GraphicsManager;

}