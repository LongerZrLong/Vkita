#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 v_TexCoord;

layout(location = 0) out vec4 color;

void main()
{
    color = texture(texSampler, v_TexCoord);
}
