#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>

#include <ecs/components/Transform.hpp>
#include <ecs/components/CameraComponent.hpp>

inline glm::mat4 getViewMatrix(const Transform& transform)
{
    return glm::inverse(transform.getMatrix());
}

inline glm::mat4 getProjectionMatrix(const CameraComponent& cam)
{
    if (cam.isOrtho)
    {
        float size = 10.0f;
        return glm::ortho(
            -size * cam.aspect,
            size * cam.aspect,
            -size,
            size,
            cam.nearPlane,
            cam.farPlane
        );
    }

    return glm::perspective(
        glm::radians(cam.fov),
        cam.aspect,
        cam.nearPlane,
        cam.farPlane
    );
}