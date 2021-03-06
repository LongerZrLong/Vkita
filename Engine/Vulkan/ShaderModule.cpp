#include "ShaderModule.h"

#include "Core/FileSystem.h"

namespace VKT::Vulkan {

    ShaderModule::ShaderModule(const Device &device, const std::string &path)
        : ShaderModule(device, g_FileSystem->ReadFile(path))
    {
    }

    ShaderModule::ShaderModule(const Device &device, const std::vector<char> &code)
        : m_Device(device)
    {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        Check(vkCreateShaderModule(device, &createInfo, nullptr, &m_VkShaderModule));
    }

    ShaderModule::~ShaderModule()
    {
        if (m_VkShaderModule != nullptr)
        {
            vkDestroyShaderModule(m_Device, m_VkShaderModule, nullptr);
            m_VkShaderModule = nullptr;
        }
    }

    VkPipelineShaderStageCreateInfo ShaderModule::CreateShaderStage(VkShaderStageFlagBits stage) const
    {
        VkPipelineShaderStageCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        createInfo.stage = stage;
        createInfo.module = m_VkShaderModule;
        createInfo.pName = "main";

        return createInfo;
    }

}
