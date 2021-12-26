#include "GraphicsManager.h"

#include "Config.h"

#include "Application/Application.h"

#include "Common/Log.h"

// Temporary
#include <array>

namespace VKT {

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    int GraphicsManager::Initialize()
    {
        const auto validationLayers = enableValidationLayers
                                      ? std::vector<const char*>{ "VK_LAYER_KHRONOS_validation" }
                                      : std::vector<const char*>();

        m_Instance = CreateScope<Vulkan::Instance>(g_App->GetWindow(), validationLayers, VK_API_VERSION_1_2);
        m_DebugUtilsMessenger = enableValidationLayers? CreateScope<Vulkan::DebugUtilsMessenger>(*m_Instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) : nullptr;
        m_Surface = CreateScope<Vulkan::Surface>(*m_Instance);

        CreateDevice();

        // Create swap chain and command buffers
        CreateSwapChain();

        CreateSyncObjects();

        LogVulkanInfo();

        // TODO: Delete Testing Code
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

        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo{};
        ubo.Model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.View = glm::mat4(1.0f);

        // Draw Call
        {
            vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline->GetPipeline().GetVkHandle());

            VkBuffer vertexBuffers[] = { m_VertexBuffer->GetBuffer().GetVkHandle() };
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(vkCommandBuffer, 0, 1, vertexBuffers, offsets);

            vkCmdBindIndexBuffer(vkCommandBuffer, m_IndexBuffer->GetBuffer().GetVkHandle(), 0, VK_INDEX_TYPE_UINT32);

            VkDescriptorSet descriptorSets[] = { m_DescriptorSetManager->GetDescriptorSet().GetVkHandle() };
            vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    m_GraphicsPipeline->GetPipelineLayout().GetVkHandle(), 0, 1,
                                    descriptorSets, 0, nullptr);

            vkCmdDrawIndexed(vkCommandBuffer, m_IndexBuffer->GetCount(), 1, 0, 0, 0);
        }

        ubo.Proj = glm::mat4(1.0f);

        m_UniformBuffer->Update(ubo);

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
