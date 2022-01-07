#include "GameLogic.h"

namespace VKT {

    void GameLogic::OnUpdate()
    {
        for (auto layer : m_LayerStack)
        {
            layer->OnUpdate();
        }
    }

    void GameLogic::OnEvent(Event &event)
    {
        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
        {
            --it;
            (*it)->OnEvent(event);

            if (event.Handled) break;
        }
    }

    void GameLogic::PushLayer(Layer *layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void GameLogic::PushOverlay(Layer *overlay)
    {
        m_LayerStack.PushOverlay(overlay);
        overlay->OnAttach();
    }

    void GameLogic::EraseLayer(Layer *layer)
    {
        m_LayerStack.EraseLayer(layer);
        layer->OnDetach();
    }

    void GameLogic::EraseOverlay(Layer *overlay)
    {
        m_LayerStack.EraseOverlay(overlay);
        overlay->OnDetach();
    }
}
