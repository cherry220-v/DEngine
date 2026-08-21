#include "ParticleSystem.hpp"

#include <ecs/components/ParticleEmitterComponent.hpp>
#include <ecs/components/Transform.hpp>
#include <render/RenderContext.hpp>
#include <core/EngineContext.hpp>
#include <math/Random.h>

#include <glm/gtx/norm.hpp>

void ParticleSystem::init(EngineContext* ctx)
{
}

void ParticleSystem::update(Scene& scene, float dt)
{
    for (auto& entity : scene.view<ParticleEmitterComponent>())
    {
        auto* emitter = scene.getComponent<ParticleEmitterComponent>(entity);
        auto* transform = scene.getComponent<Transform>(entity);
        auto& runtime = state[entity];

        runtime.accumulator += dt;
        if (emitter->spawnRate <= 0.0f)
            continue;
        float interval = 1.0f / emitter->spawnRate;

        while (runtime.accumulator >= interval && runtime.particles.size() < emitter->maxParticles)
        {
            runtime.accumulator -= interval;
            runtime.particles.push_back(spawn(*emitter, *transform));
        }

        for (auto& p : runtime.particles)
        {
            p.life -= dt;
            p.position += p.velocity * dt;
            p.velocity += emitter->acceleration * dt;
        }

        std::erase_if(runtime.particles,
            [](auto& p) { return p.life <= 0; });
    }
}

const std::vector<Particle>& ParticleSystem::getParticles(Entity e) const
{
    auto it = state.find(e);
    if (it == state.end()) return {};
    return it->second.particles;
}

Particle ParticleSystem::spawn(const ParticleEmitterComponent& emitter, const Transform& transform)
{
    Particle particle;

    particle.life =
        emitter.particleLifetime;

    particle.maxLife =
        emitter.particleLifetime;

    particle.alive = true;

    glm::vec3 randomOffset =
    {
        randomFloat(-emitter.spread, emitter.spread),
        randomFloat(-emitter.spread, emitter.spread),
        randomFloat(-emitter.spread, emitter.spread)
    };

    glm::vec3 dir =
        emitter.direction +
        randomOffset;

    if (glm::length2(dir) < 0.00001f)
        dir = glm::vec3(0, 1, 0);
    else
        dir = glm::normalize(dir);

    float speed =
        randomFloat(
            emitter.speedMin,
            emitter.speedMax
        );

    particle.velocity =
        dir * speed;

    particle.size =
        randomFloat(
            emitter.sizeMin,
            emitter.sizeMax
        );

    switch (emitter.spawnShape)
    {
    case ParticleSpawnShape::Point:
    {
        particle.position =
            transform.position;

        break;
    }

    case ParticleSpawnShape::Box:
    {
        particle.position =
            transform.position +
            randomVec(
                emitter.spawnBoxMin,
                emitter.spawnBoxMax
            );

        break;
    }

    case ParticleSpawnShape::Sphere:
    {
        glm::vec3 dir =
            glm::normalize(
                randomVec(
                    glm::vec3(-1.0f),
                    glm::vec3(1.0f)
                )
            );

        float radius =
            emitter.spawnRadius *
            cbrtf(randomFloat(0.0f, 1.0f));

        particle.position =
            transform.position +
            dir * radius;

        break;
    }

    case ParticleSpawnShape::Cylinder:
    {
        float angle =
            randomFloat(
                0.0f,
                glm::two_pi<float>()
            );

        float radius =
            randomFloat(
                0.0f,
                emitter.spawnRadius
            );

        float height =
            randomFloat(
                -emitter.spawnHeight * 0.5f,
                emitter.spawnHeight * 0.5f
            );

        particle.position =
            transform.position +
            glm::vec3(
                cos(angle) * radius,
                height,
                sin(angle) * radius
            );

        break;
    }
    }
    return particle;
}

