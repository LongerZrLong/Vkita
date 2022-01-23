#version 450

// Camera
layout (set = 0, binding = 0) uniform Camera
{
    mat4 View;
    mat4 Proj;
} camera;

// Model
layout (set = 1, binding = 0) uniform PerBatch
{
    mat4 Model;
} perBatch;

// Material
layout (set = 2, binding = 0) uniform MatPara
{
    vec4 DiffColor;
    vec4 SpecColor;
    float Shininess;
} matPara;

layout (set = 2, binding = 1) uniform sampler2D diffSampler;
layout (set = 2, binding = 2) uniform sampler2D specSampler;

// Light
struct Light
{
    int Type;

    vec3 Position;
    vec3 Direction;
    vec3 Up;

    float AttenConstant;
    float AttenLinear;
    float AttenQuadratic;

    vec3 DiffuseColor;
    vec3 SpecularColor;
    vec3 AmbientColor;

    float AngleInnerCone;
    float AngleOuterCone;

    vec2 Size;
};

layout (set = 3, binding = 0) uniform LightBlock
{
    Light light;
} lightBlock;

layout (location = 0) in vec3 v_WorldPos;
layout (location = 1) in vec2 v_TexCoord;
layout (location = 2) in vec3 v_Normal;

layout (location = 0) out vec4 fragColor;

vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(v_Normal);

    vec3 camWorldPos = vec3(inverse(camera.View) * vec4(0.0, 0.0, 0.0, 1.0));
    vec3 viewDir = normalize(camWorldPos - v_WorldPos);

    vec3 result = vec3(0.0);

    switch (lightBlock.light.Type)
    {
        case 1:     // Directional lgiht
        {
            result += CalcDirLight(lightBlock.light, norm, viewDir);
            break;
        }

        case 2:     // Point light
        {
            result += CalcPointLight(lightBlock.light, norm, v_WorldPos, viewDir);
            break;
        }

        case 3:     // Spot light
        {
            result += CalcSpotLight(lightBlock.light, norm, v_WorldPos, viewDir);
            break;
        }
    }

    fragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.Direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matPara.Shininess);

    // combine results
    vec3 diffColor = vec3(texture(diffSampler, v_TexCoord) * matPara.DiffColor);
    vec3 specColor = vec3(texture(specSampler, v_TexCoord) * matPara.SpecColor);

    vec3 diffuse = light.DiffuseColor * diff * diffColor;
    vec3 specular = light.SpecularColor * spec * specColor;
    vec3 ambient = light.AmbientColor * diffColor;

    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.Position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matPara.Shininess);

    // attenuation
    float distance = length(light.Position - fragPos);
    float attenuation = 1.0 / (light.AttenConstant + light.AttenLinear * distance + light.AttenQuadratic * (distance * distance));

    // combine results
    vec3 diffColor = vec3(texture(diffSampler, v_TexCoord) * matPara.DiffColor);
    vec3 specColor = vec3(texture(specSampler, v_TexCoord) * matPara.SpecColor);

    vec3 diffuse = light.DiffuseColor * diff * diffColor;
    vec3 specular = light.SpecularColor * spec * specColor;
    vec3 ambient = light.AmbientColor * diffColor;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.Position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matPara.Shininess);

    // attenuation
    float distance = length(light.Position - fragPos);
    float attenuation = 1.0 / (light.AttenConstant + light.AttenLinear * distance + light.AttenQuadratic * (distance * distance));

    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.Direction));
    float epsilon = light.AngleInnerCone - light.AngleOuterCone;
    float intensity = clamp((theta - light.AngleOuterCone) / epsilon, 0.0, 1.0);

    // combine results
    vec3 diffColor = vec3(texture(diffSampler, v_TexCoord) * matPara.DiffColor);
    vec3 specColor = vec3(texture(specSampler, v_TexCoord) * matPara.SpecColor);

    vec3 diffuse = light.DiffuseColor * diff * diffColor;
    vec3 specular = light.SpecularColor * spec * specColor;
    vec3 ambient = light.AmbientColor * diffColor;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}