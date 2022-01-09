#version 450

layout (set = 0, binding = 0) uniform UniformBufferObject
{
    mat4 View;
    mat4 Proj;
} shaderData;

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Color;

layout (location = 0) out vec3 v_Color;

void main()
{
    gl_Position = shaderData.Proj * shaderData.View * vec4(a_Position, 1.0);
    v_Color = a_Color;
}
