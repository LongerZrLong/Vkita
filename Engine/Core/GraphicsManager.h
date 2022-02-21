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

#define MAX_LIGHTS 10

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

        Scope<Vulkan::DescriptorPool> m_DescriptorPool;

        Scope<Vulkan::PipelineLayout> m_PipelineLayout;
        Scope<Vulkan::Pipeline> m_GraphicsPipeline;

        Scope<Vulkan::ShaderModule> m_VertShader;
        Scope<Vulkan::ShaderModule> m_FragShader;

        Scope<Rendering::VertexBuffer> m_VertexBuffer;
        Scope<Rendering::IndexBuffer> m_IndexBuffer;

        Scope<Vulkan::DescriptorSetLayout> m_PerFrameDescSetLayout;
        Scope<Vulkan::DescriptorSetLayout> m_PerBatchDescSetLayout;
        Scope<Vulkan::DescriptorSetLayout> m_MaterialDescSetLayout;

        struct PerFrameContext
        {
            struct
            {
                alignas(16) glm::mat4 View;
                alignas(16) glm::mat4 Proj;
            } ViewProj;

            alignas(4) int NumLights;

            Light::Parameter Lights[MAX_LIGHTS];

        } m_PerFrameContext;

        Scope<Rendering::Buffer> m_PerFrameUbo;
        Scope<Vulkan::DescriptorSet> m_PerFrameDescSet;

        // PerBatch model matrix
        std::unordered_map<SceneNode*, Scope<Rendering::Buffer>> m_RuntimePerBatchUboDict;
        std::unordered_map<SceneNode*, Scope<Vulkan::DescriptorSet>> m_RuntimePerBatchDescSetDict;

        // PerBatch PerMesh Material
        std::vector<Scope<Vulkan::DescriptorSet>> m_MaterialDescSets;
        std::vector<Scope<Rendering::Buffer>> m_MaterialUbos;
        std::unordered_map<std::string, Scope<Rendering::Texture2D>> m_TextureDict;

        // Debug Info
        struct DebugPrimitive
        {
            size_t FirstIndex;
            size_t IndexCount;
        };
        std::unordered_map<SceneNode*, DebugPrimitive> m_DebugPrimitivesDict;

        Scope<Rendering::Buffer> m_DebugVertBuffer;
        Scope<Rendering::Buffer> m_DebugIndexBuffer;

        Scope<Vulkan::PipelineLayout> m_DebugPipelineLayout;
        Scope<Vulkan::Pipeline> m_DebugGraphicsPipeline;

        Scope<Vulkan::ShaderModule> m_DebugVertShader;
        Scope<Vulkan::ShaderModule> m_DebugFragShader;
    };

    extern GraphicsManager *g_GraphicsManager;

}