#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

struct CameraBasis
{
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 forward;
};

static CameraBasis getCameraBasis(const glm::mat4& view)
{
    glm::mat4 inv = glm::inverse(view);

    return {
        glm::normalize(glm::vec3(inv[0])),
        glm::normalize(glm::vec3(inv[1])),
        glm::normalize(glm::vec3(inv[2]))
    };
}

static float getGizmoScale(
    const glm::vec3& pos,
    const glm::mat4& view,
    float viewportHeight,
    float basePx = 120.0f)
{
    glm::vec3 camPos = glm::vec3(glm::inverse(view)[3]);
    float dist = glm::length(camPos - pos);

    return dist * (basePx / viewportHeight);
}