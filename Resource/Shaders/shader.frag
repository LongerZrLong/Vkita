#version 450

layout (set = 2, binding = 0) uniform Parameter
{
    vec4 DiffColor;
    vec4 SpecColor;
    float Shininess;
} parameter;

layout (set = 2, binding = 1) uniform sampler2D diffSampler;
layout (set = 2, binding = 2) uniform sampler2D specSampler;

layout (location = 0) in vec2 v_TexCoord;
layout (location = 1) in vec3 v_Normal;

layout (location = 0) out vec4 color;

void main()
{
    // Temporary set direction light and ambient light
    vec3 directionalLightDir = vec3(0.0, -1.0, -1.0);
    float directionalLightIntensity = 0.5;

    float ambientLightIntensity = 0.5;

    float cosine = dot(-normalize(directionalLightDir), v_Normal);

    color = (ambientLightIntensity + directionalLightIntensity * cosine) * texture(diffSampler, v_TexCoord) * parameter.DiffColor;
}
