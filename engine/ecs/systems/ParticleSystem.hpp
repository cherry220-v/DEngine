#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <ecs/registry/SystemRegistry.hpp>
#include <ecs/entity/Entity.hpp>

#include "ISystem.hpp"
#include <scene/particles/Particle.hpp>
#include <vector>

struct ParticleEmitterComponent;
struct Transform;
struct EngineContext;

class ParticleSystem : public ISystem
{
public:
    void init(EngineContext* ctx) override;
    void update(Scene& scene, float dt) override;

    const std::vector<Particle>& getParticles(Entity e) const;

    Particle spawn(const ParticleEmitterComponent& emitter, const Transform& transform);

private:
    struct Runtime
    {
        std::vector<Particle> particles;
        float accumulator = 0.0f;
    };

    std::unordered_map<Entity, Runtime> state;
};

REGISTER_SYSTEM(ParticleSystem);