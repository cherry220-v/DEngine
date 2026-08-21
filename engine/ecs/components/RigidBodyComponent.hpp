#pragma once

#include "IComponent.hpp"
#include "../registry/ComponentInfo.hpp"
#include "../registry/ComponentRegistry.hpp"

#include <physx/PxActor.h>

struct RigidBodyComponent {
    float mass = 1.0f;
    bool isStatic = false;

    physx::PxRigidActor* actor = nullptr;
};

static ComponentInfo rigidBodyComponentInfo
{
    "RigidBodyComponent",
    {
        PROPERTY(
            RigidBodyComponent,
            mass,
            PropertyType::Float
        ),
        PROPERTY(
            RigidBodyComponent,
            isStatic,
            PropertyType::Bool
        )
    }
};

REGISTER_COMPONENT(rigidBodyComponentInfo);