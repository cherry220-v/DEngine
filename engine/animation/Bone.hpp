#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/mat4x4.hpp>

struct Bone
{
    std::string name;

    int parent = -1;

    glm::mat4 inverseBindMatrix = glm::mat4(1.0f);

    glm::mat4 localTransform = glm::mat4(1.0f);
};