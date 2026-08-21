#pragma once

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

class Shader;

struct GPULight
{
    glm::vec4 position;   // xyz + range
    glm::vec4 direction;  // xyz + outerAngle
    glm::vec4 color;      // rgb + intensity

    glm::mat4 lightSpaceMatrix;

    glm::vec4 params;
    glm::vec4 shadowParams;
};
void uploadLights(Shader& shader, const std::vector<GPULight>& lights);
