#pragma once

#include <memory>
#include "../systems/ISystem.hpp"
#include <functional>

struct EngineContext;

struct SystemInfo
{
    std::function<
        std::unique_ptr<ISystem>(
            EngineContext*
        )
    > create;
};