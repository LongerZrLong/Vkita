#version 450

layout (set = 0, binding = 0) uniform UniformBufferObject
{
    mat4 View;
    mat4 Proj;
} shaderData;

layout (set = 1, binding = 0) uniform ModelMatrixUBO
{
    mat4 Model;
} primitive;

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Tangent;
layout (location = 3) in vec3 a_Bitangent;
layout (location = 4) in vec2 a_TexCoord;

layout (location = 0) out vec2 v_TexCoord;
layout (location = 1) out vec3 v_Normal;

void main()
{
    gl_Position = shaderData.Proj * shaderData.View * primitive.Model * vec4(a_Position, 1.0);
    v_TexCoord = a_TexCoord;
    v_Normal = a_Normal;
}
