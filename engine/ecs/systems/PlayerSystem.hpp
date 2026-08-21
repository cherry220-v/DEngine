#pragma once

#include <ecs/registry/SystemRegistry.hpp>
#include <core/platform/input/IInput.hpp>
#include "ISystem.hpp"

class Scene;
struct EngineContext;

class PlayerSystem : public ISystem
{
public:
    void init(EngineContext* ctx) override;

    void setInput(IInput* input);

    void update(Scene& scene, float dt) override;
private:
    IInput* m_input = nullptr;
};

REGISTER_SYSTEM(PlayerSystem);