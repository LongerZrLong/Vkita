#include "EditorCamera.h"

#include "Core/InputManager.h"

#include "Input/KeyCodes.h"

namespace VKT {

    void EditorCamera::SetViewportSize(uint32_t width, uint32_t height)
    {
        m_AspectRatio = (float)width / (float)height;
        m_ViewportWidth = width;
        m_ViewportHeight = height;

        UpdateProjection();
    }

    void EditorCamera::OnUpdate()
    {
        if (g_InputManager->IsKeyPressed(Key::LeftAlt))
        {
            glm::vec2 newMousePos = g_InputManager->GetMousePosition();
            glm::vec2 delta = (newMousePos - m_CurrentMousePos) * 0.003f;
            m_CurrentMousePos = newMousePos;

            if (g_InputManager->IsMouseButtonPressed(Mouse::ButtonLeft))
            {
                MouseOrbit(delta);
            }
            else if (g_InputManager->IsMouseButtonPressed(Mouse::ButtonMiddle))
            {
                MouseMove(delta);
            }
            else if (g_InputManager->IsMouseButtonPressed(Mouse::ButtonRight))
            {
                MouseZoom(delta.y);
            }
            else
                return;
        }

        UpdateView();
    }

    void EditorCamera::OnEvent(Event &event)
    {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<MouseScrolledEvent>(
            [this](MouseScrolledEvent &event) -> bool { return OnMouseScrollEvent(event); });
    }

    void EditorCamera::UpdateProjection()
    {
        m_Projection = glm::perspective(m_PerspectiveVerticalFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
    }

    void EditorCamera::UpdateView()
    {
        m_Position = CalcPosition();

        glm::quat orientation = GetOrientation();
        m_View = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
        m_View = glm::inverse(m_View);
    }

    void EditorCamera::MouseOrbit(glm::vec2 delta)
    {
        m_Yaw += delta.x * OrbitSpeed();
        m_Pitch += delta.y * OrbitSpeed();
    }

    void EditorCamera::MouseMove(glm::vec2 delta)
    {
        glm::vec2 moveSpeed = MoveSpeed();
        m_FocalPoint += -GetRightDirection() * delta.x * moveSpeed.x * m_Distance;
        m_FocalPoint += GetUpDirection() * delta.y * moveSpeed.y * m_Distance;
    }

    void EditorCamera::MouseZoom(float delta)
    {
        m_Distance -= delta * ZoomSpeed();
    }

    float EditorCamera::OrbitSpeed() const
    {
        return 1.0f;
    }

    glm::vec2 EditorCamera::MoveSpeed() const
    {
        float x = std::min((float)m_ViewportWidth / 1000.0f, 2.5f);
        float xFactor = 0.04f * (x * x) - 0.18f * x + 0.3f;

        float y = std::min((float)m_ViewportHeight / 1000.0f, 2.5f);
        float yFactor = 0.04f * (y * y) - 0.18f * y + 0.3f;

        return { xFactor, yFactor };
    }

    float EditorCamera::ZoomSpeed() const
    {
        float distance = m_Distance * 0.3f;
        distance = std::max(distance, 0.0f);

        float speed = distance * distance;
        speed = std::min(speed, 100.0f); // max speed = 100

        return speed;
    }

    glm::vec3 EditorCamera::GetRightDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    glm::vec3 EditorCamera::GetUpDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::vec3 EditorCamera::GetForwardDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
    }

    glm::quat EditorCamera::GetOrientation() const
    {
        return {glm::vec3(-m_Pitch, -m_Yaw, 0.0f)};
    }

    glm::vec3 EditorCamera::CalcPosition() const
    {
        return m_FocalPoint - GetForwardDirection() * m_Distance;
    }

    bool EditorCamera::OnMouseScrollEvent(MouseScrolledEvent &event)
    {
        float delta = event.GetYOffset() * 0.02f;
        MouseZoom(delta);
        UpdateView();
        return false;
    }

}