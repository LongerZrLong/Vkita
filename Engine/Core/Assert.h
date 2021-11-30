#pragma once

#include <csignal>

#include "Log.h"

#ifdef VKT_ENABLE_ASSERTS
    #define VKT_CORE_ASSERT(x, ...) { if (!(x)) { VKT_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); raise(SIGTRAP);; } }
    #define VKT_ASSERT(x, ...) { if (!(x)) { VKT_ERROR("Assertion Failed: {0}", __VA_ARGS__); raise(SIGTRAP);; } }
#else
    #define VKT_CORE_ASSERT(x, ...)
    #define VKT_ASSERT(x, ...)
#endif
