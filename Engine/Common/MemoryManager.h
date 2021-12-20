#pragma once

#include <cstddef>

#include "Interface/IRuntimeModule.h"

namespace VKT {

    class Allocator;

    class MemoryManager : public IRuntimeModule
    {
    public:
        template<typename T, typename ... Args>
        T *New(Args &&... args)
        {
            return new (Allocate(sizeof(T))) T(args...);
        }

        template<typename T>
        void Delete(T *p)
        {
            p->~T();
            Free(p, sizeof(T));
        }

    public:
        MemoryManager() = default;
        ~MemoryManager() override = default;

        int Initialize() override;
        void ShutDown() override;

        void Tick() override;

        void *Allocate(size_t size);
        void  Free(void *p, size_t size);

    private:
        static size_t       *m_BlockSizeLookup;
        static Allocator    *m_Allocators;

    private:
        static Allocator *LookUpAllocator(size_t size);

    };

    extern MemoryManager *g_MemoryManager;

}