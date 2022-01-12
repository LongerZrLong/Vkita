#version 450

layout (set = 0, binding = 0) uniform PerFrame
{
    mat4 ViewProjection;
} perFrame;

layout (set = 1, binding = 0) uniform PerBatch
{
    mat4 Model;
} perBatch;

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Color;

layout (location = 0) out vec3 v_Color;

void main()
{
    gl_Position = perFrame.ViewProjection * perBatch.Model * vec4(a_Position, 1.0);
    v_Color = a_Color;
}
