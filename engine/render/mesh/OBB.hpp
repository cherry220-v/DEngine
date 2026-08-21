#pragma once

#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>

struct OBB
{
    glm::vec3 center;
    glm::vec3 halfExtents;
    glm::mat3 rotation;

    bool contains(const glm::vec3& p) const
    {
        glm::vec3 local =
            glm::transpose(rotation) * (p - center);

        return std::abs(local.x) <= halfExtents.x &&
            std::abs(local.y) <= halfExtents.y &&
            std::abs(local.z) <= halfExtents.z;
    }
};