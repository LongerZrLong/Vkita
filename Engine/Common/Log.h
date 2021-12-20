#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include "Common/Base.h"

namespace VKT {

    class Log
    {
    public:
        static void Init();

        inline static std::shared_ptr<spdlog::logger> &GetCoreLogger() { return s_CoreLogger; };
        inline static std::shared_ptr<spdlog::logger> &GetClientLogger() { return s_ClientLogger; }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
}


#define VKT_CORE_TRACE(...)                 ::VKT::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define VKT_CORE_INFO(...)                  ::VKT::Log::GetCoreLogger()->info(__VA_ARGS__)
#define VKT_CORE_WARN(...)                  ::VKT::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define VKT_CORE_ERROR(...)                 ::VKT::Log::GetCoreLogger()->error(__VA_ARGS__)
#define VKT_CORE_FATAL(...)                 ::VKT::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define VKT_TRACE(...)                      ::VKT::Log::GetClientLogger()->trace(__VA_ARGS__)
#define VKT_INFO(...)                       ::VKT::Log::GetClientLogger()->info(__VA_ARGS__)
#define VKT_WARN(...)                       ::VKT::Log::GetClientLogger()->warn(__VA_ARGS__)
#define VKT_ERROR(...)                      ::VKT::Log::GetClientLogger()->error(__VA_ARGS__)
#define VKT_FATAL(...)                      ::VKT::Log::GetClientLogger()->critical(__VA_ARGS__)

