#pragma once

#include "IComponent.hpp"
#include "../registry/ComponentInfo.hpp"
#include "../registry/ComponentRegistry.hpp"

#include <string>

struct NameComponent : public IComponent
{
	std::string name;
};

static ComponentInfo nameComponentInfo
{
    "NameComponent",
    {
        PROPERTY(
            NameComponent,
            name,
            PropertyType::String
        ),
    },
    [](Scene* scene, Entity e) -> bool
    {
        return scene->hasComponent<NameComponent>(e);
    },

    [](Scene* scene, Entity e) -> void*
    {
        return scene->getComponent<NameComponent>(e);
    },

    [](Scene* scene, Entity e) -> void*
    {
        return &scene->addComponent<NameComponent>(e);
    }
};

REGISTER_COMPONENT(nameComponentInfo);