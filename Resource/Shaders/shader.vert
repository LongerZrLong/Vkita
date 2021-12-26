#version 450

layout(binding = 0) uniform UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Bitangent;
layout(location = 4) in vec2 a_TexCoord;

layout(location = 0) out vec2 v_TexCoord;

void main()
{
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(a_Position, 1.0);
    v_TexCoord = a_TexCoord;
}
