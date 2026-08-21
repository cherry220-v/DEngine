#pragma once

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

struct TransformData
{
    glm::vec3 position{ 0.0f };
    glm::quat rotation = glm::quat(1, 0, 0, 0);
    glm::vec3 scale{ 1.0f };

    glm::mat4 getMatrix() const
    {
        glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 R = glm::mat4_cast(rotation);
        glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);

        return T * R * S;
    }

    void updateFromMatrix(const glm::mat4& matrix)
    {
        glm::vec3 skew;
        glm::vec4 perspective;

        glm::decompose(matrix, scale, rotation, position, skew, perspective);

        rotation = glm::normalize(rotation);
    }

    glm::vec3 getRotEuler() const
    {
        return glm::degrees(glm::eulerAngles(rotation));
    }

    void setRotEuler(const glm::vec3& eulerDeg)
    {
        glm::vec3 rad = glm::radians(eulerDeg);

        rotation =
            glm::quat(glm::vec3(rad.x, rad.y, rad.z));
    }

    TransformData interpolate(
        const TransformData& a,
        const TransformData& b,
        float t)
    {
        TransformData result;

        result.position =
            glm::mix(
                a.position,
                b.position,
                t
            );

        result.rotation =
            glm::slerp(
                a.rotation,
                b.rotation,
                t
            );

        result.scale =
            glm::mix(
                a.scale,
                b.scale,
                t
            );

        return result;
    }
};