#pragma once

#include <glm/vec4.hpp>

struct GPUParticle
{
    glm::vec4 position; // position + size;
    glm::vec4 color; // rgb + intensity;

    glm::ivec4 params; // mb texture2d id
};