#pragma once

#include "IComponent.hpp"
#include "../registry/ComponentInfo.hpp"
#include "../registry/ComponentRegistry.hpp"
#include <ecs/Scene.hpp>

struct CameraComponent : public IComponent
{
    float fov = 60.0f;
    float aspect = 1.777f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

    bool isOrtho = false;
};

static ComponentInfo cameraComponentInfo
{
    "CameraComponent",
    {
        PROPERTY(
            CameraComponent,
            fov,
            PropertyType::Float
        ),

        PROPERTY(
            CameraComponent,
            nearPlane,
            PropertyType::Float
        ),

        PROPERTY(
            CameraComponent,
            farPlane,
            PropertyType::Float
        ),

        PROPERTY(
            CameraComponent,
            isOrtho,
            PropertyType::Bool
        )
    },
    [](Scene* scene, Entity e) -> bool
    {
        return scene->hasComponent<CameraComponent>(e);
    },

    [](Scene* scene, Entity e) -> void*
    {
        return scene->getComponent<CameraComponent>(e);
    },

    [](Scene* scene, Entity e) -> void*
    {
        return &scene->addComponent<CameraComponent>(e);
    }
};

REGISTER_COMPONENT(cameraComponentInfo);