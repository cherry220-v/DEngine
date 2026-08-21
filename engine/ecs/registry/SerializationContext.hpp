#pragma once

#include <ecs/entity/Entity.hpp>
#include <unordered_map>
#include <string>

class Scene;

struct SerializationContext
{
    Scene* scene = nullptr;
    std::unordered_map<std::string, Entity> entityMap = {};
};