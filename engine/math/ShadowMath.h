#pragma once
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

inline glm::mat4 createDirectionalLightSpaceMatrix(
    const glm::vec3& lightDir,
    const glm::vec3& sceneCenter,
    float orthoSize,
    float nearPlane,
    float farPlane)
{
    glm::vec3 dir = glm::normalize(lightDir);

    glm::vec3 lightPos = sceneCenter - dir * 50.0f;

    glm::vec3 up = glm::abs(dir.y) > 0.9f
        ? glm::vec3(0, 0, 1)
        : glm::vec3(0, 1, 0);

    glm::mat4 lightView =
        glm::lookAt(
            lightPos,
            sceneCenter,
            up
        );

    glm::mat4 lightProjection =
        glm::ortho(
            -orthoSize, orthoSize,
            -orthoSize, orthoSize,
            nearPlane, farPlane
        );

    return lightProjection * lightView;
}