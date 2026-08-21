#pragma once

#include "ISystem.hpp"
#include <ecs/entity/Entity.hpp>
#include <ecs/registry/SystemRegistry.hpp>

class Scene;

class CameraSystem : public ISystem
{
public:
    void init(EngineContext* context) override;
    void update(Scene& scene, float dt) override;

    void setActiveCamera(Entity e) { activeCamera = e; }

private:
    Entity activeCamera = 0;
};

REGISTER_SYSTEM(CameraSystem);