#pragma once

#include <memory>

#define BIT(x) (1 << x)

namespace VKT {

    // Scope Pointer
    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args &&... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    // Reference Pointer
    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args &&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}

#define NON_COPIABLE(ClassName) \
	ClassName(const ClassName&) = delete; \
	ClassName(ClassName&&) = delete; \
	ClassName &operator = (const ClassName&) = delete; \
	ClassName &operator = (ClassName&&) = delete;
