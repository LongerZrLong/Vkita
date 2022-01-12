#include "GraphicsManager.h"

#include <array>

#include "Config.h"
#include "FileSystem.h"
#include "SceneManager.h"
#include "PhysicsManager.h"
#include "DebugManager.h"

#include "Application/App.h"

#include "Math/Glm.h"

#include "Vulkan/Initializers.h"
#include "Vulkan/BufferUtil.h"

namespace VKT {

    int GraphicsManager::Initialize()
    {
        m_Ctx = CreateScope<Rendering::Context>();
        
        m_Ctx->Initialize();
        
        return 0;
    }

    void GraphicsManager::ShutDown()
    {
        m_Ctx->ShutDown();
    }

    void GraphicsManager::Tick()
    {
        if (g_SceneManager->IsSceneChanged())
        {
            m_Ctx->device->WaitIdle();
            InitializeGeometries();
            PreparePipeline();

            BuildCommandBuffers();

            g_SceneManager->NotifySceneIsRenderingQueued();
        }

        if (g_DebugManager->IsDebugInfoChanged())
        {
            m_Ctx->device->WaitIdle();
            InitializeDebugInfo();
            PrepareDebugPipeline();

            BuildCommandBuffers();

            g_DebugManager->NotifySceneIsRenderingQueued();
        }

        // Update model matrix for each scene node
        auto &scene = g_SceneManager->GetScene();
        for (auto &node : scene.m_SceneNodes)
        {
            UpdateRuntimeNodeModelMatrix(node);
        }

        if (!BeginFrame())
            return;

        EndFrame();

        Present();
    }

    void GraphicsManager::SetPerFrame(glm::mat4 View, glm::mat4 Projection)
    {
        Projection[1][1] *= -1;
        glm::mat4 viewProjection = Projection * View;
        m_PerFrameUbo->Update(&viewProjection);
    }

    bool GraphicsManager::BeginFrame()
    {
        vkWaitForFences(m_Ctx->device->GetVkHandle(), 1, &m_Ctx->inFlightFences[m_Ctx->currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = m_Ctx->swapChain->AcquireNextImage(m_Ctx->imageAvailableSemaphores[m_Ctx->currentFrame], &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            m_Ctx->RecreateSwapChain();
            PreparePipeline();
            BuildCommandBuffers();
            return false;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        // Check if a previous frame is using this image (i.e. there is its fence to wait on)
        if (m_Ctx->imagesInFlight[imageIndex] != nullptr)
        {
            vkWaitForFences(m_Ctx->device->GetVkHandle(), 1, &m_Ctx->imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
        }
        // Mark the image as now being in use by this frame
        m_Ctx->imagesInFlight[imageIndex] = m_Ctx->inFlightFences[m_Ctx->currentFrame];

        m_Ctx->currentImageIndex = imageIndex;

        return true;
    }

    void GraphicsManager::EndFrame()
    {
        VkSubmitInfo submitInfo = Vulkan::Initializers::submitInfo();

        VkCommandBuffer commandBuffers[] = { (*m_Ctx->cmdBuffers)[m_Ctx->currentImageIndex] };
        VkSemaphore waitSemaphores[] = {m_Ctx->imageAvailableSemaphores[m_Ctx->currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = commandBuffers;

        VkSemaphore signalSemaphores[] = {m_Ctx->renderFinishedSemaphores[m_Ctx->currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(m_Ctx->device->GetVkHandle(), 1, &m_Ctx->inFlightFences[m_Ctx->currentFrame]);

        if (vkQueueSubmit(m_Ctx->device->GetGraphicsQueue(), 1, &submitInfo, m_Ctx->inFlightFences[m_Ctx->currentFrame]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        m_Ctx->currentFrame = (m_Ctx->currentFrame + 1) % Config::kMaxInFlightFrameCount;
    }

    void GraphicsManager::Present()
    {
        size_t prevFrame = (m_Ctx->currentFrame - 1) % Config::kMaxInFlightFrameCount;
        VkResult result = m_Ctx->swapChain->QueuePresent(m_Ctx->device->GetPresentQueue(), m_Ctx->currentImageIndex, m_Ctx->renderFinishedSemaphores[prevFrame]);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            m_Ctx->RecreateSwapChain();
            PreparePipeline();
            BuildCommandBuffers();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }

    void GraphicsManager::InitializeGeometries()
    {
        auto &scene = g_SceneManager->GetScene();

        m_VertexBuffer = CreateRef<Rendering::VertexBuffer>(scene.m_Vertices);
        m_IndexBuffer = CreateRef<Rendering::IndexBuffer>(scene.m_Indices);

        // PerFrameUbo has a 4x4 ViewProjection matrix
        m_PerFrameUbo = CreateScope<Rendering::Buffer>(
            sizeof(glm::mat4),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

        // Setup model matrix uniform buffer object
        for (auto &node : scene.m_SceneNodes)
        {
            SetupRuntimePerBatchUboDict(node);
        }

        for (auto &it : scene.m_Textures)
        {
            m_TextureDict[it.first] = CreateScope<Rendering::Texture2D>(*it.second);
        }

        m_MaterialUbos.resize(scene.m_Materials.size());
        for (size_t i = 0; i < scene.m_Materials.size(); i++)
        {
            m_MaterialUbos[i] = CreateScope<Rendering::Buffer>(
                sizeof(Material::Parameter),
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

            m_MaterialUbos[i]->Update(&scene.m_Materials[i].m_Parameter);
        }

        // Set up descriptors
        // One uniform buffer for view and projection matrix
        // One uniform buffer for each scene node's model matrix
        // One uniform buffer for each material
        // Two combined image samplers (diffuse + specular) for each material
        std::vector<VkDescriptorPoolSize> poolSizes =
            {
                Vulkan::Initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1),
                Vulkan::Initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, m_RuntimePerBatchUboDict.size()),
                Vulkan::Initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, scene.m_Materials.size()),
                Vulkan::Initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2 * scene.m_Materials.size())
            };

        const uint32_t maxSetCount = 1 + m_RuntimePerBatchUboDict.size() + scene.m_Materials.size();
        VkDescriptorPoolCreateInfo descriptorPoolInfo = Vulkan::Initializers::descriptorPoolCreateInfo(poolSizes, maxSetCount);
        m_DescriptorPool = CreateRef<Vulkan::DescriptorPool>(*m_Ctx->device, &descriptorPoolInfo);

        // Descriptor set layout for per frame matrix (ViewProjection Matrix)
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings =
            {
                Vulkan::Initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0),
            };
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCI = Vulkan::Initializers::descriptorSetLayoutCreateInfo(setLayoutBindings.data(), static_cast<uint32_t>(setLayoutBindings.size()));
        m_PerFrameDescSetLayout = CreateScope<Vulkan::DescriptorSetLayout>(*m_Ctx->device, &descriptorSetLayoutCI);

        // Descriptor set layout for model matrix
        setLayoutBindings =
            {
                Vulkan::Initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0),
            };
        descriptorSetLayoutCI = Vulkan::Initializers::descriptorSetLayoutCreateInfo(setLayoutBindings.data(), static_cast<uint32_t>(setLayoutBindings.size()));
        m_PerBatchDescSetLayout = CreateScope<Vulkan::DescriptorSetLayout>(*m_Ctx->device, &descriptorSetLayoutCI);

        // Descriptor Set layout for each material
        setLayoutBindings =
            {
                Vulkan::Initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, 0),
                Vulkan::Initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1),
                Vulkan::Initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 2),
            };
        descriptorSetLayoutCI = Vulkan::Initializers::descriptorSetLayoutCreateInfo(setLayoutBindings.data(), static_cast<uint32_t>(setLayoutBindings.size()));
        m_MaterialDescSetLayout = CreateScope<Vulkan::DescriptorSetLayout>(*m_Ctx->device, &descriptorSetLayoutCI);

        // Descriptor Set for per frame uniform
        auto allocInfo = Vulkan::Initializers::descriptorSetAllocateInfo(m_DescriptorPool->GetVkHandle(), &m_PerFrameDescSetLayout->GetVkHandle(), 1);
        m_PerFrameDescSet = CreateScope<Vulkan::DescriptorSet>(*m_Ctx->device, &allocInfo);

        // Descriptor Set for each scene node model matrix
        for (auto &node : scene.m_SceneNodes)
        {
            SetupRuntimePerBatchDescSetDict(node);
        }

        // Descriptor Set for each material
        m_MaterialDescSets.resize(scene.m_Materials.size());
        for (size_t i = 0; i < scene.m_Materials.size(); i++)
        {
            allocInfo = Vulkan::Initializers::descriptorSetAllocateInfo(m_DescriptorPool->GetVkHandle(), &m_MaterialDescSetLayout->GetVkHandle(), 1);
            m_MaterialDescSets[i] = CreateScope<Vulkan::DescriptorSet>(*m_Ctx->device, &allocInfo);
        }

        // Write Descriptor Set for per frame uniform
        VkDescriptorBufferInfo bufferInfo = m_PerFrameUbo->GetDescriptor();
        m_PerFrameDescSet->Update(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &bufferInfo);

        // Write Descriptor Set for each scene node model matrix
        for (auto &node : scene.m_SceneNodes)
        {
            WriteRuntimePerBatchDescSetDict(node);
        }

        // Write Descriptor Set for each material
        for (size_t i = 0; i < m_MaterialDescSets.size(); i++)
        {
            VkDescriptorBufferInfo uboInfo = m_MaterialUbos[i]->GetDescriptor();
            m_MaterialDescSets[i]->Update(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &uboInfo);

            if (!scene.m_Materials[i].m_DiffuseTextureName.empty())
            {
                VkDescriptorImageInfo diffMap = m_TextureDict[scene.m_Materials[i].m_DiffuseTextureName]->GetDescriptor();
                m_MaterialDescSets[i]->Update(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, &diffMap);
            }
            else
            {
                VkDescriptorImageInfo defaultMap = m_TextureDict["_default"]->GetDescriptor();
                m_MaterialDescSets[i]->Update(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, &defaultMap);
            }

            if (!scene.m_Materials[i].m_SpecularTextureName.empty())
            {
                VkDescriptorImageInfo specMap = m_TextureDict[scene.m_Materials[i].m_SpecularTextureName]->GetDescriptor();
                m_MaterialDescSets[i]->Update(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, &specMap);
            }
            else
            {
                VkDescriptorImageInfo defaultMap = m_TextureDict["_default"]->GetDescriptor();
                m_MaterialDescSets[i]->Update(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, &defaultMap);
            }

        }
    }

    void GraphicsManager::PreparePipeline()
    {
        m_VertShader = CreateRef<Vulkan::ShaderModule>(*m_Ctx->device, g_FileSystem->Append(g_FileSystem->GetRoot(), "Resource/Shaders/shader.vert.spv"));
        m_FragShader = CreateRef<Vulkan::ShaderModule>(*m_Ctx->device, g_FileSystem->Append(g_FileSystem->GetRoot(), "Resource/Shaders/shader.frag.spv"));

        // Pipeline layout
        std::array<VkDescriptorSetLayout, 3> setLayouts =
            {
                m_PerFrameDescSetLayout->GetVkHandle(),
                m_PerBatchDescSetLayout->GetVkHandle(),
                m_MaterialDescSetLayout->GetVkHandle()
            };
        VkPipelineLayoutCreateInfo pipelineLayoutCI = Vulkan::Initializers::pipelineLayoutCreateInfo(setLayouts.data(), static_cast<uint32_t>(setLayouts.size()));
        m_PipelineLayout = CreateRef<Vulkan::PipelineLayout>(*m_Ctx->device, &pipelineLayoutCI);

        // Create Graphics Pipeline
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCI = Vulkan::Initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
        VkPipelineRasterizationStateCreateInfo rasterizationStateCI = Vulkan::Initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);
        VkPipelineColorBlendAttachmentState blendAttachmentStateCI = Vulkan::Initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
        VkPipelineColorBlendStateCreateInfo colorBlendStateCI = Vulkan::Initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentStateCI);
        VkPipelineDepthStencilStateCreateInfo depthStencilStateCI = Vulkan::Initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
        VkPipelineViewportStateCreateInfo viewportStateCI = Vulkan::Initializers::pipelineViewportStateCreateInfo(1, 1, 0);
        VkPipelineMultisampleStateCreateInfo multisampleStateCI = Vulkan::Initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, 0);
        const std::vector<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicStateCI = Vulkan::Initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables.data(), static_cast<uint32_t>(dynamicStateEnables.size()), 0);

        std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages
        {
            m_VertShader->CreateShaderStage(VK_SHADER_STAGE_VERTEX_BIT),
            m_FragShader->CreateShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT)
        };

        auto vertexInputBindings = Rendering::Vertex::GetBindingDescription();
        auto vertexInputAttributes = Rendering::Vertex::GetAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputStateCI = Vulkan::Initializers::pipelineVertexInputStateCreateInfo(vertexInputBindings, vertexInputAttributes);

        VkGraphicsPipelineCreateInfo pipelineCI = Vulkan::Initializers::pipelineCreateInfo(m_PipelineLayout->GetVkHandle(), m_Ctx->renderPass->GetVkHandle(), 0);
        pipelineCI.pVertexInputState = &vertexInputStateCI;
        pipelineCI.pInputAssemblyState = &inputAssemblyStateCI;
        pipelineCI.pRasterizationState = &rasterizationStateCI;
        pipelineCI.pColorBlendState = &colorBlendStateCI;
        pipelineCI.pMultisampleState = &multisampleStateCI;
        pipelineCI.pViewportState = &viewportStateCI;
        pipelineCI.pDepthStencilState = &depthStencilStateCI;
        pipelineCI.stageCount = static_cast<uint32_t>(shaderStages.size());
        pipelineCI.pStages = shaderStages.data();
        pipelineCI.pDynamicState = &dynamicStateCI;

        m_GraphicsPipeline = CreateRef<Vulkan::Pipeline>(*m_Ctx->device, &pipelineCI);
    }

    void GraphicsManager::BuildCommandBuffers()
    {
        VkClearValue clearValues[2];
        clearValues[0].color = { { 0.1f, 0.1f, 0.1f, 1.0f } };;
        clearValues[1].depthStencil = { 1.0f, 0 };

        VkRenderPassBeginInfo renderPassBeginInfo = Vulkan::Initializers::renderPassBeginInfo();
        renderPassBeginInfo.renderPass = m_Ctx->renderPass->GetVkHandle();
        renderPassBeginInfo.renderArea.offset = {0, 0};
        renderPassBeginInfo.renderArea.extent = m_Ctx->swapChain->GetVkExtent2D();
        renderPassBeginInfo.clearValueCount = 2;
        renderPassBeginInfo.pClearValues = clearValues;

        const VkViewport viewport = Vulkan::Initializers::viewport((float)m_Ctx->swapChain->GetVkExtent2D().width, (float)m_Ctx->swapChain->GetVkExtent2D().height, 0.0f, 1.0f);
        const VkRect2D scissor = Vulkan::Initializers::rect2D(m_Ctx->swapChain->GetVkExtent2D().width, m_Ctx->swapChain->GetVkExtent2D().height, 0, 0);

        auto &scene = g_SceneManager->GetScene();

        for (size_t i = 0; i < m_Ctx->cmdBuffers->GetSize(); i++)
        {
            renderPassBeginInfo.framebuffer = m_Ctx->frameBuffers[i]->GetVkHandle();

            VkCommandBuffer vkCommandBuffer = m_Ctx->cmdBuffers->Begin(i);
            vkCmdBeginRenderPass(vkCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdSetViewport(vkCommandBuffer, 0, 1, &viewport);
            vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);

            VkDeviceSize offsets[1] = { 0 };

            vkCmdBindVertexBuffers(vkCommandBuffer, 0, 1, &m_VertexBuffer->GetBuffer().GetVkHandle(), offsets);
            vkCmdBindIndexBuffer(vkCommandBuffer, m_IndexBuffer->GetBuffer().GetVkHandle(), 0, VK_INDEX_TYPE_UINT32);
            vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline->GetVkHandle());
            vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout->GetVkHandle(), 0, 1, &m_PerFrameDescSet->GetVkHandle(), 0, nullptr);

            for (auto &node : scene.m_SceneNodes)
                DrawNode(vkCommandBuffer, node);

            // If there is no debug info
            if (!m_DebugPrimitivesDict.empty() && g_DebugManager->IsDrawDebugInfo())
            {
                // Use DebugGraphicsPipeline to draw debug info
                vkCmdBindVertexBuffers(vkCommandBuffer, 0, 1, &m_DebugVertBuffer->GetBuffer().GetVkHandle(), offsets);
                vkCmdBindIndexBuffer(vkCommandBuffer, m_DebugIndexBuffer->GetBuffer().GetVkHandle(), 0, VK_INDEX_TYPE_UINT32);
                vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_DebugGraphicsPipeline->GetVkHandle());
                vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_DebugPipelineLayout->GetVkHandle(), 0, 1, &m_PerFrameDescSet->GetVkHandle(), 0, nullptr);

                for (auto &node : scene.m_SceneNodes)
                    DrawNodeDebugInfo(vkCommandBuffer, node);
            }

            vkCmdEndRenderPass(vkCommandBuffer);

            m_Ctx->cmdBuffers->End(i);
        }
    }

    void GraphicsManager::DrawNode(VkCommandBuffer vkCommandBuffer, SceneNode &node)
    {
        if (!node.m_Mesh.m_Primitives.empty())
        {
            vkCmdBindDescriptorSets(vkCommandBuffer,
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    m_PipelineLayout->GetVkHandle(), 1, 1,
                                    &m_RuntimePerBatchDescSetDict[&node]->GetVkHandle(), 0, nullptr);

            for (auto &primitive : node.m_Mesh.m_Primitives)
            {
                if (primitive.IndexCount > 0)
                {
                    vkCmdBindDescriptorSets(vkCommandBuffer,
                                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                                            m_PipelineLayout->GetVkHandle(), 2, 1,
                                            &m_MaterialDescSets[primitive.MaterialIndex]->GetVkHandle(), 0, nullptr);
                    vkCmdDrawIndexed(vkCommandBuffer, primitive.IndexCount, 1, primitive.FirstIndex, 0, 0);
                }
            }
        }

        for (auto &child : node.m_Children)
        {
            DrawNode(vkCommandBuffer, child);
        }
    }

    void GraphicsManager::SetupRuntimePerBatchUboDict(SceneNode &node)
    {
        m_RuntimePerBatchUboDict[&node] = CreateScope<Rendering::Buffer>(
            sizeof(glm::mat4),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

        for (auto &child : node.m_Children)
        {
            SetupRuntimePerBatchUboDict(child);
        }
    }

    void GraphicsManager::SetupRuntimePerBatchDescSetDict(SceneNode &node)
    {
        auto allocInfo = Vulkan::Initializers::descriptorSetAllocateInfo(m_DescriptorPool->GetVkHandle(), &m_PerBatchDescSetLayout->GetVkHandle(), 1);
        m_RuntimePerBatchDescSetDict[&node] = CreateScope<Vulkan::DescriptorSet>(*m_Ctx->device, &allocInfo);

        for (auto &child : node.m_Children)
        {
            SetupRuntimePerBatchDescSetDict(child);
        }
    }

    void GraphicsManager::WriteRuntimePerBatchDescSetDict(SceneNode &node)
    {
        // Write Descriptor Set for model matrix
        auto bufferInfo = m_RuntimePerBatchUboDict[&node]->GetDescriptor();
        m_RuntimePerBatchDescSetDict[&node]->Update(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &bufferInfo);

        for (auto &child : node.m_Children)
        {
            WriteRuntimePerBatchDescSetDict(child);
        }
    }

    void GraphicsManager::UpdateRuntimeNodeModelMatrix(SceneNode &node)
    {
        glm::mat4 modelMatrix;

        if (void *rigidBody = node.m_RigidBody)
        {
            modelMatrix = g_PhysicsManager->GetRigidBodyTransform(rigidBody);
        }
        else
        {
            modelMatrix = node.GetLocalToWorldMatrix();
        }

        m_RuntimePerBatchUboDict[&node]->Update(&modelMatrix);

        for (auto &child : node.m_Children)
        {
            UpdateRuntimeNodeModelMatrix(child);
        }
    }

    void GraphicsManager::ResetScene()
    {
        auto &scene = g_SceneManager->GetScene();

        for (auto &node : scene.m_SceneNodes)
        {
            ResetRuntimeNodeModelMatrix(node);
        }

        g_PhysicsManager->ClearRigidBodies();
        g_PhysicsManager->CreateRigidBodies();
    }

    void GraphicsManager::ResetRuntimeNodeModelMatrix(SceneNode &node)
    {
        glm::mat4 modelMatrix = node.GetLocalToWorldMatrix();

        m_RuntimePerBatchUboDict[&node]->Update(&modelMatrix);

        for (auto &child : node.m_Children)
        {
            ResetRuntimeNodeModelMatrix(child);
        }
    }

    void GraphicsManager::InitializeDebugInfo()
    {
        std::vector<DebugVertex> debugVertices;
        std::vector<uint32_t> debugIndices;
        std::unordered_map<SceneNode*, size_t> firstVertexInVertexArray;

        for (auto &it : g_DebugManager->GetVerticesMap())
        {
            size_t curSize = debugVertices.size();
            for (auto &vertex : it.second)
            {
                debugVertices.push_back(vertex);
            }
            firstVertexInVertexArray[it.first] = curSize;
        }

        for (auto &it : g_DebugManager->GetIndicesMap())
        {
            size_t curSize = debugIndices.size();
            size_t vertexStart = firstVertexInVertexArray[it.first];
            size_t indicesCount = 0;
            for (auto &index : it.second)
            {
                debugIndices.push_back(vertexStart + index);
                indicesCount++;
            }

            m_DebugPrimitivesDict[it.first].FirstIndex = curSize;
            m_DebugPrimitivesDict[it.first].IndexCount = indicesCount;
        }

        // If there is no debug info, return immediately
        if (m_DebugPrimitivesDict.empty()) return;

        // Create Vertex Buffer for Debug Info
        m_DebugVertBuffer = CreateRef<Rendering::Buffer>(sizeof(debugVertices[0]) * debugVertices.size(),
                                                           VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        Vulkan::BufferUtil::CopyFromStagingBuffer(m_Ctx->GetDevice(),
                                                  m_Ctx->GetCommandPool(),
                                                  m_DebugVertBuffer->GetBuffer(),
                                                  m_DebugVertBuffer->GetSize(),
                                                  debugVertices.data());

        // Create Index Buffer for Debug Info
        m_DebugIndexBuffer = CreateRef<Rendering::Buffer>(sizeof(debugIndices[0]) * debugIndices.size(),
                                                       VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        Vulkan::BufferUtil::CopyFromStagingBuffer(m_Ctx->GetDevice(),
                                                  m_Ctx->GetCommandPool(),
                                                  m_DebugIndexBuffer->GetBuffer(),
                                                  m_DebugIndexBuffer->GetSize(),
                                                  debugIndices.data());
    }

    void GraphicsManager::PrepareDebugPipeline()
    {
        m_DebugVertShader = CreateRef<Vulkan::ShaderModule>(*m_Ctx->device, g_FileSystem->Append(g_FileSystem->GetRoot(), "Resource/Shaders/debug.vert.spv"));
        m_DebugFragShader = CreateRef<Vulkan::ShaderModule>(*m_Ctx->device, g_FileSystem->Append(g_FileSystem->GetRoot(), "Resource/Shaders/debug.frag.spv"));

        // Pipeline layout
        std::array<VkDescriptorSetLayout, 2> setLayouts =
            {
                m_PerFrameDescSetLayout->GetVkHandle(),
                m_PerBatchDescSetLayout->GetVkHandle(),
            };
        VkPipelineLayoutCreateInfo pipelineLayoutCI = Vulkan::Initializers::pipelineLayoutCreateInfo(setLayouts.data(), static_cast<uint32_t>(setLayouts.size()));
        m_DebugPipelineLayout = CreateRef<Vulkan::PipelineLayout>(*m_Ctx->device, &pipelineLayoutCI);

        // Create Graphics Pipeline
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCI = Vulkan::Initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_LINE_LIST, 0, VK_FALSE);
        VkPipelineRasterizationStateCreateInfo rasterizationStateCI = Vulkan::Initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_LINE, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);
        VkPipelineColorBlendAttachmentState blendAttachmentStateCI = Vulkan::Initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
        VkPipelineColorBlendStateCreateInfo colorBlendStateCI = Vulkan::Initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentStateCI);
        VkPipelineDepthStencilStateCreateInfo depthStencilStateCI = Vulkan::Initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
        VkPipelineViewportStateCreateInfo viewportStateCI = Vulkan::Initializers::pipelineViewportStateCreateInfo(1, 1, 0);
        VkPipelineMultisampleStateCreateInfo multisampleStateCI = Vulkan::Initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, 0);
        const std::vector<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicStateCI = Vulkan::Initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables.data(), static_cast<uint32_t>(dynamicStateEnables.size()), 0);

        std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages
            {
                m_DebugVertShader->CreateShaderStage(VK_SHADER_STAGE_VERTEX_BIT),
                m_DebugFragShader->CreateShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT)
            };

        std::vector<VkVertexInputBindingDescription> vertInputBindings =
            {
                VKT::Vulkan::Initializers::vertexInputBindingDescription(0, sizeof(DebugVertex), VK_VERTEX_INPUT_RATE_VERTEX)
            };

        std::vector<VkVertexInputAttributeDescription> vertInputAttribs =
            {
                VKT::Vulkan::Initializers::vertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(DebugVertex, a_Position)),
                VKT::Vulkan::Initializers::vertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(DebugVertex, a_Color)),
            };

        VkPipelineVertexInputStateCreateInfo vertexInputStateCI = Vulkan::Initializers::pipelineVertexInputStateCreateInfo(vertInputBindings, vertInputAttribs);

        VkGraphicsPipelineCreateInfo pipelineCI = Vulkan::Initializers::pipelineCreateInfo(m_DebugPipelineLayout->GetVkHandle(), m_Ctx->renderPass->GetVkHandle(), 0);
        pipelineCI.pVertexInputState = &vertexInputStateCI;
        pipelineCI.pInputAssemblyState = &inputAssemblyStateCI;
        pipelineCI.pRasterizationState = &rasterizationStateCI;
        pipelineCI.pColorBlendState = &colorBlendStateCI;
        pipelineCI.pMultisampleState = &multisampleStateCI;
        pipelineCI.pViewportState = &viewportStateCI;
        pipelineCI.pDepthStencilState = &depthStencilStateCI;
        pipelineCI.stageCount = static_cast<uint32_t>(shaderStages.size());
        pipelineCI.pStages = shaderStages.data();
        pipelineCI.pDynamicState = &dynamicStateCI;

        m_DebugGraphicsPipeline = CreateRef<Vulkan::Pipeline>(*m_Ctx->device, &pipelineCI);
    }

    void GraphicsManager::DrawNodeDebugInfo(VkCommandBuffer vkCommandBuffer, SceneNode &node)
    {
        if (m_DebugPrimitivesDict.count(&node) > 0)
        {
            vkCmdBindDescriptorSets(vkCommandBuffer,
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    m_DebugPipelineLayout->GetVkHandle(),
                                    1, 1,
                                    &m_RuntimePerBatchDescSetDict[&node]->GetVkHandle(), 0, nullptr);

            vkCmdDrawIndexed(vkCommandBuffer, m_DebugPrimitivesDict[&node].IndexCount, 1, m_DebugPrimitivesDict[&node].FirstIndex, 0, 0);
        }

        for (auto &child : node.m_Children)
        {
            DrawNodeDebugInfo(vkCommandBuffer, child);
        }
    }
}
