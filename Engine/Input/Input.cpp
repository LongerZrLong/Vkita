#include "Input.h"

#include "Core/InputManager.h"

namespace VKT {

    bool Input::IsKeyPressed(KeyCode key)
    {
        return g_InputManager->IsKeyPressed(key);
    }

    bool Input::IsMouseButtonPressed(MouseCode button)
    {
        return g_InputManager->IsMouseButtonPressed(button);
    }

    glm::vec2 Input::GetMousePosition()
    {
        return g_InputManager->GetMousePosition();
    }

}
