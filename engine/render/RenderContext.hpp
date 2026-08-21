#pragma once

#include <ecs/entity/Entity.hpp>

class Scene;
class IInput;
class IRenderPlatform;

struct RenderContext
{
    Scene* scene = nullptr;
    Entity activeCamera = 0;

    IInput* input = nullptr;
    IRenderPlatform* platform = nullptr;
	float deltaTime = 0.0f;
};