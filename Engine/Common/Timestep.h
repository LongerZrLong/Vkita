#pragma once

namespace VKT {

    class Timestep
    {
    public:
        explicit Timestep(float time = 0.0f) : m_Time(time) {}

        operator float() const { return m_Time; }

        float GetSecond() const { return m_Time; }
        float GetMilliSecond() const { return m_Time * 1000.f; }

    private:
        float m_Time;
    };
}
