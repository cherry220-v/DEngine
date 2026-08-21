#pragma once

#include "IComponent.hpp"
#include <ecs/registry/ComponentInfo.hpp>
#include <ecs/registry/ComponentRegistry.hpp>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <memory>

#include <render/texture/Material.hpp>
#include <math/TransformData.hpp>

#include <assets/AssetRef.hpp>

enum class ParticleSpawnShape
{
    Point,
    Box,
    Sphere,
    Cylinder,
    Custom
};

struct ParticleEmitterComponent : IComponent
{
    ParticleSpawnShape spawnShape =
        ParticleSpawnShape::Point;

    int maxParticles = 1000;

    float spawnRate = 10.0f;
    float particleLifetime = 2.0f;

    glm::vec4 startColor{ 1.0f };
    glm::vec4 endColor{ 1.0f };

    glm::vec3 spawnBoxMin{ 0.0f };
    glm::vec3 spawnBoxMax{ 0.0f };

    float spawnRadius = 1.0f;
    float spawnHeight = 1.0f;

    glm::vec3 direction{ 0.0f, 1.0f, 0.0f };
    float spread = 0.2f;

    float speedMin = 1.0f;
    float speedMax = 3.0f;

    glm::vec3 acceleration{ 0.0f };

    float sizeMin = 1.0f;
    float sizeMax = 1.0f;

    bool active = true;
    bool loop = true;

    float duration = 0.0f;
    bool playOnStart = true;

    bool useColorOverLifetime = false;

    AssetRef<Material> material;
};

static ComponentInfo particleEmitterComponentInfo
{
    "ParticleEmitterComponent",
    {
        PROPERTY(
            ParticleEmitterComponent,
            spawnShape,
            PropertyType::Int
        ),

        PROPERTY(
            ParticleEmitterComponent,
            maxParticles,
            PropertyType::Int
        ),

        PROPERTY(
            ParticleEmitterComponent,
            spawnRate,
            PropertyType::Float
        ),

        PROPERTY(
            ParticleEmitterComponent,
            particleLifetime,
            PropertyType::Float
        ),

        PROPERTY(
            ParticleEmitterComponent,
            startColor,
            PropertyType::Vec4
        ),

        PROPERTY(
            ParticleEmitterComponent,
            endColor,
            PropertyType::Vec4
        ),

        PROPERTY(
            ParticleEmitterComponent,
            spawnBoxMin,
            PropertyType::Vec3
        ),

        PROPERTY(
            ParticleEmitterComponent,
            spawnBoxMax,
            PropertyType::Vec3
        ),

        PROPERTY(
            ParticleEmitterComponent,
            spawnRadius,
            PropertyType::Float
        ),

        PROPERTY(
            ParticleEmitterComponent,
            spawnHeight,
            PropertyType::Float
        ),

        PROPERTY(
            ParticleEmitterComponent,
            direction,
            PropertyType::Vec3
        ),

        PROPERTY(
            ParticleEmitterComponent,
            spread,
            PropertyType::Float
        ),

        PROPERTY(
            ParticleEmitterComponent,
            speedMin,
            PropertyType::Float
        ),

        PROPERTY(
            ParticleEmitterComponent,
            speedMax,
            PropertyType::Float
        ),

        PROPERTY(
            ParticleEmitterComponent,
            acceleration,
            PropertyType::Vec3
        ),

        PROPERTY(
            ParticleEmitterComponent,
            sizeMin,
            PropertyType::Float
        ),

        PROPERTY(
            ParticleEmitterComponent,
            sizeMax,
            PropertyType::Float
        ),

        PROPERTY(
            ParticleEmitterComponent,
            active,
            PropertyType::Bool
        ),

        PROPERTY(
            ParticleEmitterComponent,
            loop,
            PropertyType::Bool
        ),

        PROPERTY(
            ParticleEmitterComponent,
            duration,
            PropertyType::Float
        ),

        PROPERTY(
            ParticleEmitterComponent,
            playOnStart,
            PropertyType::Bool
        ),

        PROPERTY(
            ParticleEmitterComponent,
            useColorOverLifetime,
            PropertyType::Bool
        ),

        PROPERTY(
            ParticleEmitterComponent,
            material,
            PropertyType::Asset
        )
    },

    [](Scene* scene, Entity e) -> bool
    {
        return scene->hasComponent<ParticleEmitterComponent>(e);
    },

    [](Scene* scene, Entity e) -> void*
    {
        return scene->getComponent<ParticleEmitterComponent>(e);
    },

    [](Scene* scene, Entity e) -> void*
    {
        return &scene->addComponent<ParticleEmitterComponent>(e);
    }
};

REGISTER_COMPONENT(particleEmitterComponentInfo);