#pragma once

#include "Interface/IRuntimeModule.h"

#include "Input/KeyCodes.h"
#include "Input/MouseCodes.h"

#include "Math/Glm.h"

namespace VKT {

    class InputManager : public IRuntimeModule
    {
    public:
        InputManager() = default;
        ~InputManager() override = default;

        int Initialize() override;
        void ShutDown() override;

        void Tick() override;

        bool IsKeyPressed(KeyCode key);
        bool IsMouseButtonPressed(MouseCode button);

        glm::vec2 GetMousePosition();

    };

    extern InputManager *g_InputManager;

}
