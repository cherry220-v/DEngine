#pragma once

#include "IComponent.hpp"
#include "../registry/ComponentInfo.hpp"
#include "../registry/ComponentRegistry.hpp"
#include <ecs/entity/Entity.hpp>
#include <vector>

struct HierarchyComponent
{
    Entity parent = NullEntity;

    std::vector<Entity> children;
};

static ComponentInfo hierarchyComponentInfo
{
    "HierarchyComponent",
    {
        PROPERTY(
            HierarchyComponent,
            parent,
			PropertyType::Entity
        ),
    },
    [](Scene* scene, Entity e) -> bool
    {
        return scene->hasComponent<HierarchyComponent>(e);
    },

    [](Scene* scene, Entity e) -> void*
    {
        return scene->getComponent<HierarchyComponent>(e);
    },

    [](Scene* scene, Entity e) -> void*
    {
        return &scene->addComponent<HierarchyComponent>(e);
    }
};

REGISTER_COMPONENT(hierarchyComponentInfo);