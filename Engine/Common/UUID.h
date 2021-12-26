#pragma once

#include <cstdlib>
#include <algorithm>

namespace VKT {

    class UUID
    {
    public:
        UUID();
        UUID(uint64_t uuid);
        UUID(const UUID&) = default;

        operator uint64_t() const { return m_UUID; }

    private:
        uint64_t m_UUID;

    };

}

template<>
struct std::hash<VKT::UUID>
{
    std::size_t operator()(const VKT::UUID &uuid) const
    {
        return hash<uint64_t>()((uint64_t)uuid);
    }
};
