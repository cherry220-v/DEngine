#include "ParticlePass.hpp"

#include <glad/glad.h>

#include <ecs/Scene.hpp>
#include <ecs/components/Transform.hpp>
#include <ecs/components/CameraComponent.hpp>
#include <ecs/components/ParticleEmitterComponent.hpp>

#include <ecs/systems/ParticleSystem.hpp>

#include <ecs/registry/SystemRegistry.hpp>

#include <math/CameraMath.h>

void ParticlePass::init(
    IRenderPlatform* platform,
    IRenderDevice* device)
{
    this->platform = platform;
    this->device = device;

    createQuad();
}

void ParticlePass::execute(
    const RenderContext& ctx,
    RenderGraphStorage& graph)
{
    if (!ctx.activeCamera) return;
    auto* camTransform =
        ctx.scene->getComponent<Transform>(
            ctx.activeCamera
        );

    auto* cam =
        ctx.scene->getComponent<CameraComponent>(
            ctx.activeCamera
        );

    if (!cam || !camTransform)
        return;

    glm::mat4 view =
        getViewMatrix(*camTransform);

    glm::mat4 projection =
        getProjectionMatrix(*cam);

    glm::vec3 cameraRight =
    {
        view[0][0],
        view[1][0],
        view[2][0]
    };

    glm::vec3 cameraUp =
    {
        view[0][1],
        view[1][1],
        view[2][1]
    };

    auto emitters = ctx.scene->view<ParticleEmitterComponent, Transform>();

    device->setBlendOneAEnabled(true);
    device->setDepthTestEnabled(true);

    device->setDepthMaskEnabled(false);

    device->bindVertexArray(quadVAO);

    for (auto e : emitters)
    {

        auto* emitter = ctx.scene->getComponent<ParticleEmitterComponent>(e);
        auto* transform = ctx.scene->getComponent<Transform>(e);

        if (!bool(emitter->material))
            continue;

        auto emitterMaterial = emitter->material.get();
        auto shader = emitterMaterial->shader;

        if (!shader)
            continue;

        shader->use();

        shader->setMat4("view", view);
        shader->setMat4("projection", projection);

        shader->setVec3(
            "cameraRight",
            cameraRight
        );

        shader->setVec3(
            "cameraUp",
            cameraUp
        );

        emitterMaterial->bind();

        auto* particleSystem = ctx.scene->getSystem<ParticleSystem>();
        auto& particles = particleSystem->getParticles(e);

        for (auto& p : particles)
        {
            if (!p.alive)
                continue;

            float life =
                p.life / p.maxLife;

            shader->setVec3(
                "particlePos",
                p.position
            );

            shader->setFloat(
                "size",
                p.size
            );

            shader->setFloat(
                "life",
                life
            );

            glm::vec4 color =
                emitterMaterial->color;

            if (emitter->useColorOverLifetime)
            {
                float normalizedLife =
                    p.life /
                    p.maxLife;

                glm::vec4 lifeColor =
                    glm::mix(
                        emitter->endColor,
                        emitter->startColor,
                        normalizedLife
                    );

                color *= lifeColor;
            }

            shader->setVec4(
                "particleColor",
                color
            );

            device->drawArrays(DrawMode::Triangles, 0, 6);
        }
    }

    device->setDepthMaskEnabled(true);

    device->setBlendOneAEnabled(false);
}

void ParticlePass::createQuad() { float quad[] = { -0.5f, -0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 1.0f, 0.0f, 0.5f, 0.5f, 1.0f, 1.0f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.5f, 1.0f, 1.0f, -0.5f, 0.5f, 0.0f, 1.0f }; quadVAO = device->createVertexArray(); quadVBO = device->createVertexBuffer(quad, sizeof(quad)); device->bindVertexArray(quadVAO); device->bindVertexBuffer(quadVBO); device->setVertexAttrib(0, 2, DataType::Float, false, 4 * sizeof(float), 0); device->enableVertexAttrib(0); device->setVertexAttrib(1, 2, DataType::Float, false, 4 * sizeof(float), 2 * sizeof(float)); device->enableVertexAttrib(1); }