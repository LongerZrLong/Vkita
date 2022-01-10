#version 450

layout (set = 0, binding = 0) uniform PerFrame
{
    mat4 View;
    mat4 Proj;
} perFrame;

layout (set = 1, binding = 0) uniform PerBatch
{
    mat4 Model;
} perBatch;

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Tangent;
layout (location = 3) in vec3 a_Bitangent;
layout (location = 4) in vec2 a_TexCoord;

layout (location = 0) out vec2 v_TexCoord;
layout (location = 1) out vec3 v_Normal;

void main()
{
    gl_Position = perFrame.Proj * perFrame.View * perBatch.Model * vec4(a_Position, 1.0);
    v_TexCoord = a_TexCoord;
    v_Normal = a_Normal;
}
