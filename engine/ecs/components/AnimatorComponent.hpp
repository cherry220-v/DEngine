#pragma once

#include "IComponent.hpp"
#include "../registry/ComponentInfo.hpp"
#include "../registry/ComponentRegistry.hpp"
#include "../registry/GizmoRegistry.hpp"

#include <assets/AssetRef.hpp>

#include <animation/Animation.hpp>

#include <glm/mat4x4.hpp>
#include <memory>
#include <vector>
#include <editor/gizmos/TransformGizmo.hpp>

struct AnimatorComponent : IComponent
{
    AssetRef<AnimationClip> clip;
    float time;
    bool loop;

    std::vector<glm::mat4> boneMatrices;
};

static ComponentInfo animatorComponentInfo
{
    "AnimatorComponent",
    {
        PROPERTY(
            AnimatorComponent,
            clip,
            PropertyType::Asset
        ),

        PROPERTY(
            AnimatorComponent,
            time,
            PropertyType::Float
        ),

        PROPERTY(
            AnimatorComponent,
            loop,
            PropertyType::Bool
        )
    },
    [](Scene* scene, Entity e) -> bool
    {
        return scene->hasComponent<AnimatorComponent>(e);
    },

    [](Scene* scene, Entity e) -> void*
    {
        return scene->getComponent<AnimatorComponent>(e);
    },

    [](Scene* scene, Entity e) -> void*
    {
        return &scene->addComponent<AnimatorComponent>(e);
    }
};

REGISTER_COMPONENT(animatorComponentInfo);
//REGISTER_GIZMO("AnimatorComponent", std::make_unique<AnimatorGizmo>());