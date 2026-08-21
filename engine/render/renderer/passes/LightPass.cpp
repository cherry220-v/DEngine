#include "LightPass.hpp"

#include <ecs/Scene.hpp>
#include <ecs/components/LightComponent.hpp>
#include <ecs/components/Transform.hpp>
#include <format>
#include <iostream>

void LightPass::init(IRenderPlatform* platform, IRenderDevice* device)
{
    this->platform = platform;
    this->device = device;
}

void LightPass::setup(RenderGraphStorage& graph)
{
    lightsHandleId = graph.create("LightPass.Lights");

    writes.clear();
    writes.push_back(lightsHandleId);

    graph.publish(
        "LightPass.Lights",
        lightsHandleId
    );

    graph.set(
        lightsHandleId,
        std::make_shared<
        std::vector<GPULight>
        >()
    );
}

void LightPass::execute(
    const RenderContext& ctx,
    RenderGraphStorage& graph)
{
    std::vector<GPULight> gpuLights;

    auto entities =
        ctx.scene->view<
        Transform,
        LightComponent
        >();

    gpuLights.reserve(
        entities.size()
    );

    for (Entity entity : entities)
    {
        auto* transform =
            ctx.scene->getComponent<Transform>(
                entity
            );

        auto* light =
            ctx.scene->getComponent<LightComponent>(
                entity
            );

        if (!transform || !light)
            continue;

        glm::mat4 world =
            ctx.scene->getWorldMatrix(entity);

        glm::vec3 position =
        {
            world[3][0],
            world[3][1],
            world[3][2]
        };

        GPULight gpuLight;

        gpuLight.position =
        {
            position.x,
            position.y,
            position.z,
            light->range
        };

        gpuLight.direction =
        {
            light->direction,
            light->outerAngle
        };

        gpuLight.color =
        {
            light->color.r,
            light->color.g,
            light->color.b,
            light->intensity
        };

        gpuLight.params =
        {
            static_cast<int>(light->type),
            static_cast<float>(light->innerAngle),
            0,
            0
        };

        gpuLights.push_back(
            gpuLight
        );
    }

    graph.set(
        lightsHandleId,
        std::make_shared<std::vector<GPULight>>(
            gpuLights
        )
    );
}
