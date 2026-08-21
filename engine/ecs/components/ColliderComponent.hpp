#pragma once

#include "IComponent.hpp"
#include "../registry/ComponentInfo.hpp"
#include "../registry/ComponentRegistry.hpp"

#include <physx/PxShape.h>

struct ColliderComponent {
    enum class Type { Box, Sphere, Capsule, Model };
    Type type = Type::Box;
    glm::vec3 size = glm::vec3(1.0f);

    physx::PxShape* shape = nullptr;
};

static ComponentInfo colliderComponentInfo
{
    "ColliderComponent",
    {
        PROPERTY(
            ColliderComponent,
            type,
            PropertyType::Enum
        ),
        PROPERTY(
            ColliderComponent,
            size,
            PropertyType::Vec3
        )
    }
};

REGISTER_COMPONENT(colliderComponentInfo);