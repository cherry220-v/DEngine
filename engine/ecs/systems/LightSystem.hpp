#pragma once

#include "ISystem.hpp"
#include <ecs/registry/SystemRegistry.hpp>

class Scene;

class LightSystem : public ISystem
{
public:
    void init(EngineContext* context) override;
    void update(Scene&, float dt) override;
};

REGISTER_SYSTEM(LightSystem);