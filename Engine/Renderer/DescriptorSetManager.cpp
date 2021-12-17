#include "DescriptorSetManager.h"

#include "Renderer.h"
#include "GraphicsContext.h"
#include "DescriptorBinding.h"
#include "UniformBuffer.h"
#include "Texture2D.h"

#include "Vulkan/Device.h"
#include "Vulkan/ImageView.h"
#include "Vulkan/Sampler.h"

#include "Core/Base.h"

namespace VKT {

    DescriptorSetManager::DescriptorSetManager(const std::vector<DescriptorBinding> &descriptorBindings, const size_t maxSets)
    {
        const Vulkan::Device &device = Renderer::GetGraphicsContext().GetDevice();

        m_DescriptorPool = CreateScope<Vulkan::DescriptorPool>(device, descriptorBindings, maxSets);
        m_DescriptorSetLayout = CreateScope<Vulkan::DescriptorSetLayout>(device, descriptorBindings);
        m_DescriptorSet = CreateScope<Vulkan::DescriptorSet>(*m_DescriptorPool, *m_DescriptorSetLayout, maxSets);

        std::vector<VkWriteDescriptorSet> descriptorWrites;
        for (const auto &binding : descriptorBindings)
        {
            switch(binding.Type)
            {
                case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                {
                    auto *ptr = reinterpret_cast<UniformBuffer*>(binding.DescriptorPtr);

                    VkDescriptorBufferInfo uniformBufferInfo = {};
                    uniformBufferInfo.buffer = ptr->GetBuffer().GetVkHandle();
                    uniformBufferInfo.range = VK_WHOLE_SIZE;

                    VkWriteDescriptorSet descriptorWrite = {};
                    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptorWrite.dstSet = m_DescriptorSet->GetVkHandle();
                    descriptorWrite.dstBinding = binding.Binding;
                    descriptorWrite.dstArrayElement = 0;
                    descriptorWrite.descriptorType = binding.Type;
                    descriptorWrite.descriptorCount = 1;
                    descriptorWrite.pBufferInfo = &uniformBufferInfo;

                    vkUpdateDescriptorSets(device.GetVkHandle(), 1, &descriptorWrite, 0, nullptr);

                    break;
                }

                case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                {
                    auto *ptr = reinterpret_cast<Texture2D*>(binding.DescriptorPtr);

                    VkDescriptorImageInfo imageInfo{};
                    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    imageInfo.imageView = ptr->GetImageView().GetVkHandle();
                    imageInfo.sampler = ptr->GetSampler().GetVkHandle();

                    VkWriteDescriptorSet descriptorWrite = {};
                    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptorWrite.dstSet = m_DescriptorSet->GetVkHandle();
                    descriptorWrite.dstBinding = binding.Binding;
                    descriptorWrite.dstArrayElement = 0;
                    descriptorWrite.descriptorType = binding.Type;
                    descriptorWrite.descriptorCount = 1;
                    descriptorWrite.pImageInfo = &imageInfo;

                    vkUpdateDescriptorSets(device.GetVkHandle(), 1, &descriptorWrite, 0, nullptr);

                    break;
                }

                default:
                {
                    throw std::runtime_error("unsupported descriptor type!");
                }

            }

        }

    }

    DescriptorSetManager::~DescriptorSetManager()
    {
        m_DescriptorPool.reset();
        m_DescriptorSetLayout.reset();
    }

}
