#version 450

struct Camera
{
    mat4 View;
    mat4 Proj;
};

layout (set = 0, binding = 0) uniform PerFrame
{
    Camera u_Camera;
} perFrame;

layout (set = 1, binding = 0) uniform PerBatch
{
    mat4 u_Model;
} perBatch;

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Color;

layout (location = 0) out vec3 v_Color;

void main()
{
    gl_Position = perFrame.u_Camera.Proj * perFrame.u_Camera.View * perBatch.u_Model * vec4(a_Position, 1.0);
    v_Color = a_Color;
}
