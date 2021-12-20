#pragma once

namespace VKT {

    class IRuntimeModule
    {
    public:
        virtual ~IRuntimeModule() = default;

        virtual int Initialize() = 0;
        virtual void ShutDown() = 0;

        virtual void Tick() = 0;

    };

}
