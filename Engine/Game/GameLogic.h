#pragma once

#include "LayerStack.h"

#include "Interface/IRuntimeModule.h"

namespace VKT {

    class GameLogic : public IRuntimeModule
    {
    public:
        int Initialize() override { return 0; }
        void ShutDown() override {}
        void Tick() override { OnUpdate(); }

        void OnUpdate();
        void OnEvent(Event &event);

        void PushLayer(Layer *layer);
        void PushOverlay(Layer *overlay);

        void EraseLayer(Layer *layer);
        void EraseOverlay(Layer *overlay);

    private:
        LayerStack m_LayerStack;

    };

    extern GameLogic *g_GameLogic;

}