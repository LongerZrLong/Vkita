#pragma once

#include <string>
#include <utility>

#include "Common/Base.h"
#include "Common/Timestep.h"

#include "Event.h"

namespace VKT {

    class Layer
    {
    public:
        explicit Layer(std::string name = "Layer")
            : m_DebugName(std::move(name)) {}

        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}

        virtual void OnUpdate(Timestep ts) {}
        virtual void OnEvent(Event &event) {}

        inline const std::string &GetName() const { return m_DebugName; }

    protected:
        std::string m_DebugName;
    };
}
