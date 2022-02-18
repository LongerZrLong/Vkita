#version 450

// MAX_LIGHTS in GraphicsManager.h
#define MAX_LIGHTS 10

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
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Tangent;
layout (location = 3) in vec3 a_Bitangent;
layout (location = 4) in vec2 a_TexCoord;

layout (location = 0) out vec3 v_WorldPos;
layout (location = 1) out vec2 v_TexCoord;
layout (location = 2) out vec3 v_Normal;

void main()
{
    gl_Position = perFrame.u_Camera.Proj * perFrame.u_Camera.View * perBatch.u_Model * vec4(a_Position, 1.0);

    v_WorldPos = vec3(perBatch.u_Model * vec4(a_Position, 1.0));
    v_TexCoord = a_TexCoord;
    v_Normal = vec3(transpose(inverse(perBatch.u_Model)) * vec4(a_Normal, 1.0));
}
