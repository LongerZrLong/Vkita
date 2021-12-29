#pragma once

#include <functional>

#include "Common.h"
#include "CommandPool.h"
#include "CommandBuffers.h"

namespace VKT::Vulkan {

    class SingleTimeCommands
    {
    public:
        static void Submit(const Device &device, const CommandPool& commandPool, const std::function<void(VkCommandBuffer)>& action)
        {
            CommandBuffers commandBuffers(device, commandPool, 1);

            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            vkBeginCommandBuffer(commandBuffers[0], &beginInfo);

            action(commandBuffers[0]);

            vkEndCommandBuffer(commandBuffers[0]);

            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffers[0];

            const auto graphicsQueue = device.GetGraphicsQueue();

            vkQueueSubmit(graphicsQueue, 1, &submitInfo, nullptr);
            vkQueueWaitIdle(graphicsQueue);
        }
    };

}
