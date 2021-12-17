#pragma once

#include <string>
#include <vector>

#include "Common.h"

namespace VKT::Vulkan {

    class Device;

    class ShaderModule
    {
    public:
        NON_COPIABLE(ShaderModule);

        ShaderModule(const Device &device, const std::string &filename);
        ShaderModule(const Device &device, const std::vector<char> &code);
        ~ShaderModule();

        const Device &GetDevice() const { return m_Device; }

        VkPipelineShaderStageCreateInfo CreateShaderStage(VkShaderStageFlagBits stage) const;

    private:
        VULKAN_HANDLE(VkShaderModule, m_VkShaderModule);

        const Device &m_Device;

    };

}
