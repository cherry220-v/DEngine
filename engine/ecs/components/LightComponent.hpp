#pragma once

#include "IComponent.hpp"
#include <ecs/registry/ComponentInfo.hpp>
#include <ecs/registry/ComponentRegistry.hpp>

#include <glm/vec3.hpp>

enum class LightCType {
    Directional,
    Point,
    Spot
};

struct LightComponent : IComponent
{
    LightCType type = LightCType::Point;

    glm::vec3 direction;

    glm::vec3 color{ 1.0f };
    float intensity = 1.0f;

    float range = 10.0f;

    float innerAngle = 20.0f;
    float outerAngle = 30.0f;

    bool flicker = false;

    float flickerSpeed = 10.0f;
    float flickerAmount = 0.2f;

    float currentIntensity = 1.0f;
};

static ComponentInfo lightComponentInfo
{
    "LightComponent",
    {
        PROPERTY(
            LightComponent,
            type,
            PropertyType::Enum
        ),

        PROPERTY(
            LightComponent,
            direction,
            PropertyType::Vec3
        ),

        PROPERTY(
            LightComponent,
            color,
            PropertyType::Vec3
        ),

        PROPERTY(
            LightComponent,
            intensity,
            PropertyType::Float
        ),

        PROPERTY(
            LightComponent,
            range,
            PropertyType::Float
        ),

        PROPERTY(
            LightComponent,
            innerAngle,
            PropertyType::Float
        ),

        PROPERTY(
            LightComponent,
            outerAngle,
            PropertyType::Float
        ),

        PROPERTY(
            LightComponent,
            flicker,
            PropertyType::Bool
        ),

        PROPERTY(
            LightComponent,
            flickerSpeed,
            PropertyType::Float
        ),

        PROPERTY(
            LightComponent,
            flickerAmount,
            PropertyType::Float
        )
    },
    [](Scene* scene, Entity e) -> bool
    {
        return scene->hasComponent<LightComponent>(e);
    },

    [](Scene* scene, Entity e) -> void*
    {
        return scene->getComponent<LightComponent>(e);
    },

    [](Scene* scene, Entity e) -> void*
    {
        return &scene->addComponent<LightComponent>(e);
    }
};

REGISTER_COMPONENT(lightComponentInfo);