#pragma once

#include "IComponent.hpp"
#include "../registry/ComponentInfo.hpp"
#include "../registry/ComponentRegistry.hpp"
#include "../registry/GizmoRegistry.hpp"

#include <ecs/Scene.hpp>

#include <glm/vec3.hpp>
#include <nlohmann/json.hpp>

#include <math/TransformData.hpp>

struct Transform :
    public IComponent,
    public TransformData
{
	glm::mat4 worldMatrix{ 1.0f };
    bool dirty = false;
};

static ComponentInfo transformInfo
{
    "Transform",
    {
        PROPERTY(
            Transform,
            position,
            PropertyType::Vec3
        ),

        PROPERTY(
            Transform,
            rotation,
            PropertyType::Vec3
        ),

        PROPERTY(
            Transform,
            scale,
            PropertyType::Vec3
        )
    },

    [](Scene* scene, Entity e) -> bool
    {
        return scene->hasComponent<Transform>(e);
    },

    [](Scene* scene, Entity e) -> void*
    {
        return scene->getComponent<Transform>(e);
    },

    [](Scene* scene, Entity e) -> void*
    {
        return &scene->addComponent<Transform>(e);
    }
};

REGISTER_COMPONENT(transformInfo);