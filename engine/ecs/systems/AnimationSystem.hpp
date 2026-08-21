#pragma once

#include "ISystem.hpp"

#include <ecs/registry/SystemRegistry.hpp>

class Scene;
struct AnimatorComponent;
struct SkeletonComponent;

class AnimationSystem : public ISystem
{
public:
    void init(EngineContext* context);
    void update(Scene& scene, float dt) override;

private:
    void updateAnimator(
        AnimatorComponent& animator,
        SkeletonComponent& skeleton,
        float dt
    );
};

REGISTER_SYSTEM(AnimationSystem);