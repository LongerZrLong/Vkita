#pragma once

namespace VKT {

    enum LightType : int
    {
        Undefined = 0, Directional, Point, Spot, Ambient, Area,
    };

    class SceneNode;

    class Light
    {
    public:
        SceneNode *m_Node = nullptr;

        bool m_IsCastShadow = true;

        struct Parameter
        {
            alignas(4) LightType Type = LightType::Undefined;

            alignas(16) glm::vec3 Position = {0.0f, 0.0f, 0.0f};
            alignas(16) glm::vec3 Direction = {0.0f, 0.0f, 0.0f};
            alignas(16) glm::vec3 Up = {0.0f, 0.0f, 0.0f};

            alignas(4) float AttenConstant = 0.0f;
            alignas(4) float AttenLinear = 0.0f;
            alignas(4) float AttenQuadratic = 1.0f;

            alignas(16) glm::vec3 DiffuseColor = {0.0f, 0.0f, 0.0f};
            alignas(16) glm::vec3 SpecularColor = {0.0f, 0.0f, 0.0f};
            alignas(16) glm::vec3 AmbientColor = {0.0f, 0.0f, 0.0f};

            alignas(4) float AngleInnerCone = 0.0f;
            alignas(4) float AngleOuterCone = 0.0f;

            alignas(8) glm::vec2 Size = {0.0f, 0.0f};

        } m_Parameter;

    };
}