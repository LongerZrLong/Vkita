#version 450

layout (set = 1, binding = 0) uniform MaterialUBO
{
    bool HasDiffMap;
    bool HasSpecMap;
    vec4 DiffColor;
    vec4 SpecColor;
} materialData;

layout (set = 1, binding = 1) uniform sampler2D diffSampler;
layout (set = 1, binding = 2) uniform sampler2D specSampler;

layout(location = 0) in vec2 v_TexCoord;

layout(location = 0) out vec4 color;

void main()
{
    color = texture(diffSampler, v_TexCoord);
}
