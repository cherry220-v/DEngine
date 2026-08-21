#include "LightSystem.hpp"

#include <core/EngineContext.hpp>
#include <ecs/Scene.hpp>
#include <ecs/components/LightComponent.hpp>
#include <math/Random.h>

void LightSystem::init(EngineContext* context)
{
}

void LightSystem::update(Scene& scene, float dt)
{
    static float time = 0.0f;
    time += dt;

    for (auto entity : scene.view<LightComponent>())
    {
        auto* light =
            scene.getComponent<LightComponent>(entity);

        if (!light->flicker)
        {
            light->currentIntensity =
                light->intensity;
            continue;
        }

        light->currentIntensity =
            light->intensity *
            randomFloat(
                1.0f - light->flickerAmount,
                1.0f + light->flickerAmount
            );
    }
}