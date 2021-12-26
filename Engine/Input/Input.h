#pragma once

#include <Math/Glm.h>

#include "KeyCodes.h"
#include "MouseCodes.h"

#include "Core/Base.h"

namespace VKT {

    class Input
    {
    public:
        static bool IsKeyPressed(KeyCode key);
        static bool IsMouseButtonPressed(MouseCode button);

        static glm::vec2 GetMousePosition();
    };
}
