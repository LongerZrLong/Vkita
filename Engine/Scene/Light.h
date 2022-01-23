#pragma once

namespace VKT {

    enum class LightType
    {
        Undefined = 0, Directional, Point, Spot, Ambient, Area,
    };

    class SceneNode;

    class Light
    {
    public:
        SceneNode *Node = nullptr;

        bool IsCastShadow = true;

        LightType Type = LightType::Undefined;

        glm::vec3 Position = {0.0f, 0.0f, 0.0f};
        glm::vec3 Direction = {0.0f, 0.0f, 0.0f};
        glm::vec3 Up = {0.0f, 0.0f, 0.0f};

        float AttenConstant = 0.0f;
        float AttenLinear = 0.0f;
        float AttenQuadratic = 1.0f;

        glm::vec3 DiffuseColor = {0.0f, 0.0f, 0.0f};
        glm::vec3 SpecularColor = {0.0f, 0.0f, 0.0f};
        glm::vec3 AmbientColor = {0.0f, 0.0f, 0.0f};

        float AngleInnerCone = 0.0f;
        float AngleOuterCone = 0.0f;

        glm::vec2 Size = {0.0f, 0.0f};

    };
}