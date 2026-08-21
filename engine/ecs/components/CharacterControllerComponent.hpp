#pragma once

#include "IComponent.hpp"
#include "../registry/ComponentInfo.hpp"
#include "../registry/ComponentRegistry.hpp"

#include <physx/characterkinematic/PxController.h>

struct CharacterControllerComponent : public IComponent
{
    float moveSpeed = 5.0f;
    float jumpForce = 6.0f;

    bool grounded = false;

    //float gravity = 9.91f;
    float gravity = 0.3f;

    physx::PxController* controller = nullptr;
};