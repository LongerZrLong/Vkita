#pragma once

#include <string>
#include <vector>

#include "Common.h"
#include "Device.h"

namespace VKT::Vulkan {

    class ShaderModule
    {
    public:
        NON_COPIABLE(ShaderModule);

        ShaderModule(const Device &device, const std::string &path);
        ShaderModule(const Device &device, const std::vector<char> &code);
        ~ShaderModule();

        VkPipelineShaderStageCreateInfo CreateShaderStage(VkShaderStageFlagBits stage) const;

    private:
        VULKAN_HANDLE(VkShaderModule, m_VkShaderModule);

        const Device &m_Device;

    };

}
