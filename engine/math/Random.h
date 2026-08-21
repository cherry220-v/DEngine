#pragma once

#include <glm/vec3.hpp>

inline float randomFloat(float min, float max)
{
    return min +
        static_cast<float>(rand()) /
        static_cast<float>(RAND_MAX) *
        (max - min);
}

inline glm::vec3 randomVec(const glm::vec3& min, const glm::vec3& max)
{
    auto randf = [](float a, float b)
        {
            return a + (float(rand()) / RAND_MAX) * (b - a);
        };

    return glm::vec3(
        randf(min.x, max.x),
        randf(min.y, max.y),
        randf(min.z, max.z)
    );
}