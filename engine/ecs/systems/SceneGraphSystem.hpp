#pragma once

#include "ISystem.hpp"
#include <ecs/entity/Entity.hpp>
#include <glm/mat4x4.hpp>
#include <ecs/registry/SystemRegistry.hpp>

class SceneGraphSystem : public ISystem
{
public:
    void init(EngineContext* context) override;
    void update(Scene& scene, float dt) override;
private:
    void updateNode(
        Scene* scene,
        Entity entity,
        const glm::mat4& parentMatrix
    );
};

REGISTER_SYSTEM(SceneGraphSystem);