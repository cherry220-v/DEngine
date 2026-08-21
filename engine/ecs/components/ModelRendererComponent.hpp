#pragma once

#include "IComponent.hpp"
#include <ecs/registry/ComponentInfo.hpp>
#include <ecs/registry/ComponentRegistry.hpp>

#include <models/Model.hpp>
#include <render/texture/Material.hpp>
#include <memory>

#include <assets/AssetRef.hpp>

struct ModelRendererComponent : public IComponent
{
    AssetRef<Model> model;
    AssetRef<Material> material;
};

static ComponentInfo modelRendererComponentInfo
{
    "ModelRendererComponent",
    {
        PROPERTY(
            ModelRendererComponent,
            model,
            PropertyType::Asset
        ),

        PROPERTY(
            ModelRendererComponent,
            material,
            PropertyType::Asset
        ),
    },
    [](Scene* scene, Entity e) -> bool
    {
        return scene->hasComponent<ModelRendererComponent>(e);
    },

    [](Scene* scene, Entity e) -> void*
    {
        return scene->getComponent<ModelRendererComponent>(e);
    },

    [](Scene* scene, Entity e) -> void*
    {
        return &scene->addComponent<ModelRendererComponent>(e);
    }
};

REGISTER_COMPONENT(modelRendererComponentInfo);