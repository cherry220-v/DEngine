#include "AnimationSystem.hpp"

#include <core/EngineContext.hpp>

#include <ecs/Scene.hpp>
#include "../components/AnimatorComponent.hpp"
#include "../components/SkeletonComponent.hpp"

void AnimationSystem::update(Scene& scene, float dt)
{
    auto view =
        scene.view<
        AnimatorComponent,
        SkeletonComponent>();

    for (auto e : view)
    {
        auto* animator =
            scene.getComponent<AnimatorComponent>(e);

        auto* skeleton =
            scene.getComponent<SkeletonComponent>(e);

        if (!animator || !skeleton)
            continue;

        updateAnimator(
            *animator,
            *skeleton,
            dt
        );
    }
}

void AnimationSystem::updateAnimator(
    AnimatorComponent& animator,
    SkeletonComponent& skeleton,
    float dt)
{
    if (!animator.clip)
        return;

    animator.time += dt;

    auto animatorClip = animator.clip.get();

    if (animator.loop)
    {
        while (animator.time > animatorClip->duration)
            animator.time -= animatorClip->duration;
    }

    auto skeletonPtr = skeleton.skeleton.get();

    const auto& bones = skeletonPtr->bones;

    animator.boneMatrices.resize(
        bones.size(),
        glm::mat4(1.0f)
    );

    for (size_t i = 0; i < bones.size(); i++)
    {
        animator.boneMatrices[i] =
            bones[i].localTransform *
            bones[i].inverseBindMatrix;
    }
}
