#pragma once

#include <ecs/entity/Entity.hpp>
#include "CameraComponent.hpp"
#include "../registry/ComponentInfo.hpp"
#include "../registry/ComponentRegistry.hpp"
#include <glm/vec3.hpp>

struct PlayerComponent : public IComponent
{
    Entity camera = 0;

    float speed = 1.0f;
    float shiftSpeed = 2.5f;
    float moveSpeed = 1.0f;
    float sensitivity = 0.1f;

    float yaw = -90.0f;
    float pitch = 0.0f;

    glm::vec3 front{ 0.0f, 0.0f, -1.0f };
    glm::vec3 right{ 1.0f, 0.0f, 0.0f };
    glm::vec3 up{ 0.0f, 1.0f, 0.0f };

    void setActive(bool b) { IN_GAME = b; }
    bool isActive() { return IN_GAME; }

private:
    bool IN_GAME = false;
};

static ComponentInfo playerComponentInfo
{
    "PlayerComponent",
    {
        PROPERTY(
            PlayerComponent,
            camera,
            PropertyType::Entity
        ),
        PROPERTY(
            PlayerComponent,
            moveSpeed,
            PropertyType::Float
        ),
        PROPERTY(
            PlayerComponent,
            shiftSpeed,
            PropertyType::Float
        ),
        PROPERTY(
            PlayerComponent,
            sensitivity,
            PropertyType::Float
        ),
        PROPERTY(
            PlayerComponent,
            yaw,
            PropertyType::Float
        ),
        PROPERTY(
            PlayerComponent,
            pitch,
            PropertyType::Float
        ),
        PROPERTY(
            PlayerComponent,
            camera,
            PropertyType::Entity
        ),
    },

    [](Scene* scene, Entity e) -> bool
    {
        return scene->hasComponent<PlayerComponent>(e);
    },

    [](Scene* scene, Entity e) -> void*
    {
        return scene->getComponent<PlayerComponent>(e);
    },

    [](Scene* scene, Entity e) -> void*
    {
        return &scene->addComponent<PlayerComponent>(e);
    }
};

REGISTER_COMPONENT(playerComponentInfo);