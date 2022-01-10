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
        void ResetRuntimeNodeModelMatrix(SceneNode &node);

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

        struct PerFrame
        {
            Scope<Rendering::Buffer> buffer;
            struct Values
            {
                alignas(16) glm::mat4 View;
                alignas(16) glm::mat4 Proj;
            } values;
        } m_PerFrame;

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

        std::unordered_map<SceneNode*, Scope<Rendering::Buffer>> m_RuntimeModelMatrixUBOMap;
        std::unordered_map<SceneNode*, Scope<Vulkan::DescriptorSet>> m_RuntimeModelMatrixDescSetMap;

        std::vector<Scope<Vulkan::DescriptorSet>> m_MaterialDescSets;
        std::vector<Scope<Rendering::Buffer>> m_MaterialUniformBuffers;
        std::unordered_map<std::string, Scope<Rendering::Texture2D>> m_Textures;

        // Debug Info
        struct DebugPrimitive
        {
            size_t FirstIndex;
            size_t IndexCount;
        };
        std::unordered_map<SceneNode*, DebugPrimitive> m_DebugPrimitivesMap;

        Ref<Rendering::Buffer> m_DebugVertBuffer;
        Ref<Rendering::Buffer> m_DebugIndexBuffer;

        Ref<Vulkan::PipelineLayout> m_DebugPipelineLayout;
        Ref<Vulkan::Pipeline> m_DebugGraphicsPipeline;

        Ref<Vulkan::ShaderModule> m_DebugVertShader;
        Ref<Vulkan::ShaderModule> m_DebugFragShader;
    };

    extern GraphicsManager *g_GraphicsManager;

}