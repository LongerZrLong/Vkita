#include "Image.h"

#include "SingleTimeCommands.h"

namespace VKT::Vulkan {

    Image::Image(const Device &device, const VkExtent2D extent, const VkFormat format)
        : Image(device, extent, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
    {
    }

    Image::Image(const Device &device, const VkExtent2D extent, const VkFormat format, const VkImageTiling tiling, const VkImageUsageFlags usageFlags)
        : m_Device(device),
          m_VkExtent2D(extent),
          m_VkFormat(format),
          m_VkImageTiling(tiling),
          m_VkImageLayout(VK_IMAGE_LAYOUT_UNDEFINED)
    {
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = extent.width;
        imageInfo.extent.height = extent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = m_VkImageLayout;
        imageInfo.usage = usageFlags;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0; // Optional

        Check(vkCreateImage(device, &imageInfo, nullptr, &m_VkImage));
    }

    Image::Image(Image &&other) noexcept
        : m_Device(other.m_Device),
          m_VkExtent2D(other.m_VkExtent2D),
          m_VkFormat(other.m_VkFormat),
          m_VkImageLayout(other.m_VkImageLayout),
          m_VkImage(other.m_VkImage)
    {
        other.m_VkImage = nullptr;
    }

    Image::~Image()
    {
        if (m_VkImage != nullptr)
        {
            vkDestroyImage(m_Device, m_VkImage, nullptr);
            m_VkImage = nullptr;
        }
    }

    DeviceMemory Image::AllocateMemory(const VkMemoryPropertyFlags propertyFlags) const
    {
        const auto requirements = GetVkMemoryRequirements();
        DeviceMemory memory(m_Device, requirements.size, requirements.memoryTypeBits, 0, propertyFlags);

        Check(vkBindImageMemory(m_Device, m_VkImage, memory, 0));

        return memory;
    }

    VkMemoryRequirements Image::GetVkMemoryRequirements() const
    {
        VkMemoryRequirements requirements;
        vkGetImageMemoryRequirements(m_Device, m_VkImage, &requirements);
        return requirements;
    }

    void Image::TransitionImageLayout(const Device &device, const CommandPool &commandPool, VkImageLayout newLayout)
    {
        SingleTimeCommands::Submit(device, commandPool, [&](VkCommandBuffer commandBuffer)
        {
            VkImageMemoryBarrier barrier = {};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = m_VkImageLayout;
            barrier.newLayout = newLayout;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = m_VkImage;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;

            if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            {
                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

                if (DepthBuffer::HasStencilComponent(m_VkFormat))
                {
                    barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
                }
            }
            else
            {
                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            }

            VkPipelineStageFlags sourceStage;
            VkPipelineStageFlags destinationStage;

            if (m_VkImageLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
            {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            }
            else if (m_VkImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
            {
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            }
            else if (m_VkImageLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            }
            else
            {
                throw std::invalid_argument("unsupported layout transition");
            }

            vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        });

        m_VkImageLayout = newLayout;
    }

    void Image::CopyFrom(const Device &device, const CommandPool &commandPool, const Buffer &buffer)
    {
        SingleTimeCommands::Submit(device, commandPool, [&](VkCommandBuffer commandBuffer)
        {
            VkBufferImageCopy region = {};
            region.bufferOffset = 0;
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;
            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = 1;
            region.imageOffset = { 0, 0, 0 };
            region.imageExtent = { m_VkExtent2D.width, m_VkExtent2D.height, 1 };

            vkCmdCopyBufferToImage(commandBuffer, buffer, m_VkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
        });
    }

}
