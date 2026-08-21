#pragma once

#include <glm/vec3.hpp>

struct Particle
{
    glm::vec3 position;
    glm::vec3 velocity;

    float life;
    float maxLife;

    float size = 1.0f;

    bool alive = true;
};