#pragma once

#include "../systems/ISystem.hpp"
#include "SystemInfo.hpp"
#include <string>
#include <vector>

#define REGISTER_SYSTEM(T) \
    static bool _reg_##T = []() { \
        SystemRegistry::get().registerSystem<T>(); \
        return true; \
    }()

class SystemRegistry
{
public:
    static SystemRegistry& get() { static SystemRegistry s; return s; }

public:
    template<typename T, typename... Args>
    void registerSystem(Args&&... args)
    {
        m_factories.emplace_back([]() -> std::unique_ptr<ISystem>
            {
                return std::make_unique<T>();
            });
    }

    const std::vector<std::function<std::unique_ptr<ISystem>()>>& factories() { return m_factories; };

private:
    SystemRegistry() = default;
    ~SystemRegistry() = default;

    SystemRegistry(
        const SystemRegistry&
    ) = delete;

    SystemRegistry& operator=(
        const SystemRegistry&
        ) = delete;

private:
    std::vector<std::function<std::unique_ptr<ISystem>()>> m_factories;
};
