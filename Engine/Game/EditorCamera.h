#pragma once

#include "Camera.h"

#include "Event/Event.h"
#include "Event/MouseEvent.h"

namespace VKT {

    class EditorCamera : public Camera
    {
    public:
        EditorCamera() = default;
        EditorCamera(const EditorCamera&) = default;
        virtual ~EditorCamera() = default;

        void OnUpdate();
        void OnEvent(Event &event);

        glm::mat4 GetViewProjection() const { return m_Projection * m_View; }
        const glm::mat4 &GetView() const { return m_View; }

        void SetViewportSize(unsigned int width, unsigned int height);

        void SetFocalPoint(glm::vec3 focalPoint) { m_FocalPoint = focalPoint; UpdateView(); }
        void SetDistance(float distance) { m_Distance = distance; UpdateView(); }

    private:
        void UpdateProjection();
        void UpdateView();

    private:
        void MouseOrbit(glm::vec2 delta);
        void MouseMove(glm::vec2 delta);
        void MouseZoom(float delta);

    private:
        float OrbitSpeed() const;
        glm::vec2 MoveSpeed() const;
        float ZoomSpeed() const;

    private:
        glm::vec3 GetRightDirection() const;
        glm::vec3 GetUpDirection() const;
        glm::vec3 GetForwardDirection() const;
        glm::quat GetOrientation() const;

        glm::vec3 CalcPosition() const;

    private:
        bool OnMouseScrollEvent(MouseScrolledEvent &event);

    private:
        glm::mat4 m_View;
        glm::vec3 m_Position;
        glm::vec3 m_FocalPoint = {0.0f, 0.0f, 0.0f};

        glm::vec2 m_CurrentMousePos = {0.0f, 0.0f};
        float m_Pitch = 0.0f;
        float m_Yaw = 0.0f;
        float m_Distance = 10.0f;

        float m_PerspectiveVerticalFOV = glm::radians(45.0f);
        float m_PerspectiveNear = 0.01f;
        float m_PerspectiveFar = 1000.f;

        float m_AspectRatio = 0.0f;
        uint32_t m_ViewportWidth, m_ViewportHeight;
    };

}