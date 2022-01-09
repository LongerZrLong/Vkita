#include "InputManager.h"

#include <GLFW/glfw3.h>

#include "Application/App.h"

namespace VKT {

    int InputManager::Initialize()
    {
        return 0;
    }

    void InputManager::ShutDown()
    {
    }

    void InputManager::Tick()
    {
        glfwPollEvents();
    }

    bool InputManager::IsKeyPressed(KeyCode key)
    {
        auto window = static_cast<GLFWwindow*>(g_App->GetWindow().GetNativeWindow());
        auto state = glfwGetKey(window, static_cast<int32_t>(key));
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool InputManager::IsMouseButtonPressed(MouseCode button)
    {
        auto window = static_cast<GLFWwindow*>(g_App->GetWindow().GetNativeWindow());
        auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
        return state == GLFW_PRESS;
    }

    glm::vec2 InputManager::GetMousePosition()
    {
        auto window = static_cast<GLFWwindow*>(g_App->GetWindow().GetNativeWindow());
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);

        return { xPos, yPos };
    }
}