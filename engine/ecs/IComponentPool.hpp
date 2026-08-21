#pragma once

#include "entity/Entity.hpp"

class IComponentPool
{
public:
    virtual ~IComponentPool() = default;

    virtual void remove(Entity entity) = 0;
};