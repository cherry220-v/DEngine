#pragma once

#include "PropertyMeta.hpp"

#include <ecs/Scene.hpp>

#include <string>
#include <vector>

#define PROPERTY(type, field, propType) \
{                                       \
    #field,                             \
    offsetof(type, field),              \
    propType                            \
}

struct EditorContext;

struct ComponentInfo
{
    std::string name;
    std::vector<PropertyMeta> properties;

    bool (*hasComponent)(
        Scene* scene,
        Entity entity
        );

    void* (*getComponent)(
        Scene* scene,
        Entity entity
        );

    void* (*addComponent)(
        Scene* scene,
        Entity entity
        );
};