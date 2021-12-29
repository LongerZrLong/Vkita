#include "GraphicsManager.h"

#include "Config.h"
#include "Log.h"
#include "Image.h"
#include "FileSystem.h"

#include "Application/Application.h"

#include "Math/Glm.h"

#include "Vulkan/Initializers.h"
#include "Vulkan/ImageView.h"
#include "Vulkan/Sampler.h"
#include "Vulkan/ShaderModule.h"

// Temporary
#include <array>

namespace VKT {

    struct UniformBufferObject
    {
        alignas(16) glm::mat4 Model;
        alignas(16) glm::mat4 View;
        alignas(16) glm::mat4 Proj;
    };

    int GraphicsManager::Initialize()
    {
        const auto validationLayers = Config::kEnableValidationLayers
                                      ? std::vector<const char*>{ "VK_LAYER_KHRONOS_validation" }
                                      : std::vector<const char*>();

        m_Instance = CreateScope<Vulkan::Instance>(g_App->GetWindow(), validationLayers, VK_API_VERSION_1_2);
        m_DebugUtilsMessenger = Config::kEnableValidationLayers?
            CreateScope<Vulkan::DebugUtilsMessenger>(*m_Instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) : nullptr;
        m_Surface = CreateScope<Vulkan::Surface>(*m_Instance);

        CreateDevice();

        // Create swap chain and command buffers
        CreateSwapChain();

        CreateSyncObjects();

        LogVulkanInfo();

        // TODO: Delete Testing Code
        const std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f, 0.0f}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0.0, 0.0}},
            {{0.5f, -0.5f, 0.0f}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1.0, 0.0}},
            {{0.5f, 0.5f, 0.0f}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1.0, 1.0}},
            {{-0.5f, 0.5f, 0.0f}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0.0, 1.0}},
        };

        const std::vector<uint32_t> indices = {
            0, 1, 2, 2, 3, 0
        };

        m_VertexBuffer = CreateRef<VertexBuffer>(vertices);
        m_IndexBuffer = CreateRef<IndexBuffer>(indices);

        m_UniformBuffer = CreateRef<VulkanBuffer>(sizeof(UniformBufferObject),
                                                  VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

        UniformBufferObject ubo = {};
        ubo.Model = glm::mat4(1.0f);
        ubo.View = glm::mat4(1.0f);
        ubo.Proj = glm::mat4(1.0f);
        m_UniformBuffer->Update(&ubo);

        Image img = Image(g_FileSystem->Append(g_FileSystem->GetRoot(), "Resource/Textures/Checkerboard.png"));
        m_CheckerBoardTex = CreateRef<Texture2D>(img);

        m_VertShader = CreateRef<Shader>(g_FileSystem->Append(g_FileSystem->GetRoot(), "Resource/Shaders/shader.vert.spv"));
        m_FragShader = CreateRef<Shader>(g_FileSystem->Append(g_FileSystem->GetRoot(), "Resource/Shaders/shader.frag.spv"));
        
        // Create Pipeline Layout
        std::vector<VkDescriptorPoolSize> poolSizes = {
            Vulkan::Initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, m_SwapChain->GetVkImages().size()),
            Vulkan::Initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, m_SwapChain->GetVkImages().size()),
        };

        const uint32_t maxSetCount = (1 + 1) * m_SwapChain->GetVkImages().size();
        VkDescriptorPoolCreateInfo descriptorPoolInfo = Vulkan::Initializers::descriptorPoolCreateInfo(poolSizes, maxSetCount);
        vkCreateDescriptorPool(m_Device->GetVkHandle(), &descriptorPoolInfo, nullptr, &m_VkDescriptorPool);

        // Descriptor set layout
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
            Vulkan::Initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0),
            Vulkan::Initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1)
        };
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCI = Vulkan::Initializers::descriptorSetLayoutCreateInfo(setLayoutBindings.data(), static_cast<uint32_t>(setLayoutBindings.size()));

        vkCreateDescriptorSetLayout(m_Device->GetVkHandle(), &descriptorSetLayoutCI, nullptr, &m_VkDescriptorSetLayout);

        // Pipeline layout
        std::array<VkDescriptorSetLayout, 1> setLayouts = { m_VkDescriptorSetLayout };
        VkPipelineLayoutCreateInfo pipelineLayoutCI = Vulkan::Initializers::pipelineLayoutCreateInfo(setLayouts.data(), static_cast<uint32_t>(setLayouts.size()));
        vkCreatePipelineLayout(m_Device->GetVkHandle(), &pipelineLayoutCI, nullptr, &m_VkPipelineLayout);

        // Descriptor set
        VkDescriptorSetAllocateInfo allocInfo = Vulkan::Initializers::descriptorSetAllocateInfo(m_VkDescriptorPool, &m_VkDescriptorSetLayout, 1);
        vkAllocateDescriptorSets(m_Device->GetVkHandle(), &allocInfo, &m_VkDescriptorSet);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = m_CheckerBoardTex->GetImageView().GetVkHandle();
        imageInfo.sampler = m_CheckerBoardTex->GetSampler().GetVkHandle();

        std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
            Vulkan::Initializers::writeDescriptorSet(m_VkDescriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &m_UniformBuffer->GetDescriptor()),
            Vulkan::Initializers::writeDescriptorSet(m_VkDescriptorSet, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, &imageInfo)
            };

        vkUpdateDescriptorSets(m_Device->GetVkHandle(), writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
        
        // Create Graphics Pipeline
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCI = Vulkan::Initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
        VkPipelineRasterizationStateCreateInfo rasterizationStateCI = Vulkan::Initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE, 0);
        VkPipelineColorBlendAttachmentState blendAttachmentStateCI = Vulkan::Initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
        VkPipelineColorBlendStateCreateInfo colorBlendStateCI = Vulkan::Initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentStateCI);
        VkPipelineDepthStencilStateCreateInfo depthStencilStateCI = Vulkan::Initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
        VkPipelineViewportStateCreateInfo viewportStateCI = Vulkan::Initializers::pipelineViewportStateCreateInfo(1, 1, 0);
        VkPipelineMultisampleStateCreateInfo multisampleStateCI = Vulkan::Initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, 0);
        const std::vector<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicStateCI = Vulkan::Initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables.data(), static_cast<uint32_t>(dynamicStateEnables.size()), 0);

        std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{
            m_VertShader->GetShaderModule().CreateShaderStage(VK_SHADER_STAGE_VERTEX_BIT),
            m_FragShader->GetShaderModule().CreateShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT)
        };
        
        const std::vector<VkVertexInputBindingDescription> vertexInputBindings = {
            Vulkan::Initializers::vertexInputBindingDescription(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX),
        };
        const std::vector<VkVertexInputAttributeDescription> vertexInputAttributes = {
            Vulkan::Initializers::vertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, a_Position)),
            Vulkan::Initializers::vertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, a_Normal)),
            Vulkan::Initializers::vertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, a_Tangent)),
            Vulkan::Initializers::vertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, a_Bitangent)),
            Vulkan::Initializers::vertexInputAttributeDescription(0, 4, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, a_TexCoord)),
        };
        
        VkPipelineVertexInputStateCreateInfo vertexInputStateCI = Vulkan::Initializers::pipelineVertexInputStateCreateInfo(vertexInputBindings, vertexInputAttributes);

        VkGraphicsPipelineCreateInfo pipelineCI = Vulkan::Initializers::pipelineCreateInfo(m_VkPipelineLayout, m_RenderPass->GetVkHandle(), 0);
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

        vkCreateGraphicsPipelines(m_Device->GetVkHandle(), nullptr, 1, &pipelineCI, nullptr, &m_VkGraphicsPipeline);

        return 0;
    }

    void GraphicsManager::ShutDown()
    {
        DeviceWaitIdle();

        DeleteSwapChain();
        DeleteSyncObjects();
    }

    void GraphicsManager::Tick()
    {
        if (!BeginFrame())
            return;

        Draw();

        EndFrame();

        Present();
    }

    void GraphicsManager::Draw()
    {
        // TODO: Delete Testing Code
        VkCommandBuffer vkCommandBuffer = m_CommandBuffers->Begin(m_CurrentImageIndex);

        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = m_RenderPass->GetVkHandle();
        renderPassBeginInfo.framebuffer = m_FrameBuffers[m_CurrentImageIndex]->GetVkHandle();
        renderPassBeginInfo.renderArea.offset = {0, 0};
        renderPassBeginInfo.renderArea.extent = m_SwapChain->GetVkExtent2D();

        std::array<VkClearValue, 2> clearValues = {};
        clearValues[0].color = {{0.1f, 0.1f, 0.1f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassBeginInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(vkCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        const VkViewport viewport = Vulkan::Initializers::viewport((float)m_SwapChain->GetVkExtent2D().width, (float)m_SwapChain->GetVkExtent2D().height, 0.0f, 1.0f);
        const VkRect2D scissor = Vulkan::Initializers::rect2D(m_SwapChain->GetVkExtent2D().width, m_SwapChain->GetVkExtent2D().height, 0, 0);

        vkCmdSetViewport(vkCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);

        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo{};
        ubo.Model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.View = glm::mat4(1.0f);
        ubo.Proj = glm::mat4(1.0f);

        m_UniformBuffer->Update(&ubo);

        // Draw Call
        {
            vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VkGraphicsPipeline);

            VkBuffer vertexBuffers[] = { m_VertexBuffer->GetBuffer().GetVkHandle() };
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(vkCommandBuffer, 0, 1, vertexBuffers, offsets);

            vkCmdBindIndexBuffer(vkCommandBuffer, m_IndexBuffer->GetBuffer().GetVkHandle(), 0, VK_INDEX_TYPE_UINT32);

            vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    m_VkPipelineLayout, 0, 1,
                                    &m_VkDescriptorSet, 0, nullptr);

            vkCmdDrawIndexed(vkCommandBuffer, m_IndexBuffer->GetCount(), 1, 0, 0, 0);
        }

        vkCmdEndRenderPass(vkCommandBuffer);

        m_CommandBuffers->End(m_CurrentImageIndex);
    }

    void GraphicsManager::DeviceWaitIdle()
    {
        m_Device->WaitIdle();
    }

    const std::vector<VkExtensionProperties> &GraphicsManager::GetExtensions() const
    {
        return m_Instance->GetVkExtensionProps();
    }

    const std::vector<VkLayerProperties> &GraphicsManager::GetLayers() const
    {
        return m_Instance->GetVkLayerProps();
    }

    const std::vector<VkPhysicalDevice> &GraphicsManager::GetPhysicalDevices() const
    {
        return m_Instance->GetVkPhysicalDevices();
    }

    void GraphicsManager::SetPhysicalDevice(VkPhysicalDevice physicalDevice)
    {
        if (m_Device)
        {
            throw std::logic_error("physical device has already been set");
        }

        std::vector<const char*> requiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef __APPLE__
        requiredExtensions.push_back("VK_KHR_portability_subset");
#endif

        // TODO: Currently enable Anisotropy in Device and Sampler separately
        VkPhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        deviceFeatures.sampleRateShading = VK_TRUE;

        m_Device = CreateScope<Vulkan::Device>(physicalDevice, *m_Surface, requiredExtensions, deviceFeatures, nullptr);
        m_CommandPool = CreateScope<Vulkan::CommandPool>(*m_Device, m_Device->GetGraphicsFamilyIndex(), true);
    }

    void GraphicsManager::CreateDevice()
    {
        const auto &physicalDevices = GetPhysicalDevices();
        const auto result = std::find_if(physicalDevices.begin(), physicalDevices.end(), [](const VkPhysicalDevice& device)
        {
            // We want a device with geometry shader support.
            VkPhysicalDeviceFeatures deviceFeatures;
            vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

            // We want a device with a graphics queue.
            const auto queueFamilies = Vulkan::GetEnumerateVector(device, vkGetPhysicalDeviceQueueFamilyProperties);
            const auto hasGraphicsQueue = std::find_if(queueFamilies.begin(), queueFamilies.end(), [](const VkQueueFamilyProperties& queueFamily)
            {
                return queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT;
            });

            return hasGraphicsQueue != queueFamilies.end();
        });

        if (result == physicalDevices.end())
        {
            throw std::runtime_error("cannot find a suitable device");
        }

        VkPhysicalDeviceProperties2 deviceProp{};
        deviceProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        vkGetPhysicalDeviceProperties2(*result, &deviceProp);

        SetPhysicalDevice(*result);
    }

    void GraphicsManager::CreateSwapChain()
    {
        // Wait until the window is visible.
        while (g_App->IsMinimized())
        {
            g_App->GetWindow().WaitForEvents();
        }

        m_SwapChain = CreateScope<Vulkan::SwapChain>(*m_Device, Config::kVkPresentMode);
        m_DepthBuffer = CreateScope<Vulkan::DepthBuffer>(*m_CommandPool, m_SwapChain->GetVkExtent2D());
        m_RenderPass = CreateScope<Vulkan::RenderPass>(*m_SwapChain, *m_DepthBuffer, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_LOAD_OP_CLEAR);

        for (const auto &imageView : m_SwapChain->GetImageViews())
        {
            m_FrameBuffers.emplace_back(CreateScope<Vulkan::FrameBuffer>(*imageView, *m_RenderPass));
        }

        m_CommandBuffers = CreateScope<Vulkan::CommandBuffers>(*m_CommandPool, m_FrameBuffers.size());

        m_ImagesInFlight.resize(m_SwapChain->GetVkImages().size(), VK_NULL_HANDLE);
    }

    void GraphicsManager::DeleteSwapChain()
    {
        m_CommandBuffers.reset();
        m_FrameBuffers.clear();
        m_RenderPass.reset();
        m_DepthBuffer.reset();
        m_SwapChain.reset();
    }

    void GraphicsManager::CreateSyncObjects()
    {
        m_ImageAvailableSemaphores.resize(Config::kMaxInFlightFrameCount);
        m_RenderFinishedSemaphores.resize(Config::kMaxInFlightFrameCount);
        m_InFlightFences.resize(Config::kMaxInFlightFrameCount);
        m_ImagesInFlight.resize(m_SwapChain->GetVkImages().size(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < Config::kMaxInFlightFrameCount; i++)
        {
            if (vkCreateSemaphore(m_Device->GetVkHandle(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(m_Device->GetVkHandle(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(m_Device->GetVkHandle(), &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    void GraphicsManager::DeleteSyncObjects()
    {
        for (size_t i = 0; i < Config::kMaxInFlightFrameCount; i++)
        {
            vkDestroySemaphore(m_Device->GetVkHandle(), m_RenderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(m_Device->GetVkHandle(), m_ImageAvailableSemaphores[i], nullptr);
            vkDestroyFence(m_Device->GetVkHandle(), m_InFlightFences[i], nullptr);
        }
    }

    void GraphicsManager::RecreateSwapChain()
    {
        m_Device->WaitIdle();
        DeleteSwapChain();
        CreateSwapChain();
    }

    bool GraphicsManager::BeginFrame()
    {
        vkWaitForFences(m_Device->GetVkHandle(), 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(m_Device->GetVkHandle(), m_SwapChain->GetVkHandle(), UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapChain();
            return false;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        // Check if a previous frame is using this image (i.e. there is its fence to wait on)
        if (m_ImagesInFlight[imageIndex] != nullptr)
        {
            vkWaitForFences(m_Device->GetVkHandle(), 1, &m_ImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
        }
        // Mark the image as now being in use by this frame
        m_ImagesInFlight[imageIndex] = m_InFlightFences[m_CurrentFrame];

        m_CurrentImageIndex = imageIndex;

        return true;
    }

    void GraphicsManager::EndFrame()
    {
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkCommandBuffer commandBuffers[] = { (*m_CommandBuffers)[m_CurrentImageIndex] };
        VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = commandBuffers;

        VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(m_Device->GetVkHandle(), 1, &m_InFlightFences[m_CurrentFrame]);

        if (vkQueueSubmit(m_Device->GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        m_CurrentFrame = (m_CurrentFrame + 1) % Config::kMaxInFlightFrameCount;
    }

    void GraphicsManager::Present()
    {
        size_t prevFrame = (m_CurrentFrame - 1) % Config::kMaxInFlightFrameCount;
        VkSemaphore waitSemaphores[] = { m_RenderFinishedSemaphores[prevFrame] };

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = waitSemaphores;

        VkSwapchainKHR swapChains[] = {m_SwapChain->GetVkHandle()};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &m_CurrentImageIndex;

        VkResult result = vkQueuePresentKHR(m_Device->GetPresentQueue(), &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            RecreateSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }

    void GraphicsManager::LogVulkanInfo()
    {
        VKT_CORE_INFO("Vulkan SDK Header Version: {}", VK_HEADER_VERSION);

        VKT_CORE_INFO("Vulkan Instance Extensions: ");
        for (const auto &extension : GetExtensions())
            VKT_CORE_INFO("-  {} ({})", extension.extensionName, Vulkan::Version(extension.specVersion));

        VKT_CORE_INFO("Vulkan Instance Layers: ");
        for (const auto &layer : GetLayers())
            VKT_CORE_INFO("-  {} ({}) : {}", layer.layerName, Vulkan::Version(layer.specVersion), layer.description);

        VKT_CORE_INFO("Vulkan Devices: ");
        for (const auto &device : GetPhysicalDevices())
        {
            VkPhysicalDeviceDriverProperties driverProp{};
            driverProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;

            VkPhysicalDeviceProperties2 deviceProp{};
            deviceProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
            deviceProp.pNext = &driverProp;

            vkGetPhysicalDeviceProperties2(device, &deviceProp);

            VkPhysicalDeviceFeatures features;
            vkGetPhysicalDeviceFeatures(device, &features);

            const auto& prop = deviceProp.properties;

            const Vulkan::Version vulkanVersion(prop.apiVersion);
            const Vulkan::Version driverVersion(prop.driverVersion, prop.vendorID);

            VKT_CORE_INFO("-  [{}] {} '{}' ({}: vulkan {}, driver {} {} - {})",
                          prop.deviceID,
                          Vulkan::Strings::VendorId(prop.vendorID),
                          prop.deviceName,
                          Vulkan::Strings::DeviceType(prop.deviceType),
                          vulkanVersion,
                          driverProp.driverName,
                          driverProp.driverInfo,
                          driverVersion);
        }
    }
}
