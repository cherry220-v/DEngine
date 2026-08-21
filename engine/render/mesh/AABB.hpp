#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <algorithm>

#include <editor/DrawTypes.h>

struct AABB {
    glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 max = glm::vec3(-std::numeric_limits<float>::max());

    // Метод для расширения границ одной точкой
    void grow(const glm::vec3& point) {
        min = glm::min(min, point);
        max = glm::max(max, point);
    }

    // Метод для объединения с другим AABB
    void merge(const AABB& other) {
        min = glm::min(min, other.min);
        max = glm::max(max, other.max);
    }
};

inline AABB transformAABB(const AABB& localBox, const glm::mat4& transform) {
    AABB worldBox;

    glm::vec3 corners[8] = {
        {localBox.min.x, localBox.min.y, localBox.min.z},
        {localBox.max.x, localBox.min.y, localBox.min.z},
        {localBox.min.x, localBox.max.y, localBox.min.z},
        {localBox.max.x, localBox.max.y, localBox.min.z},
        {localBox.min.x, localBox.min.y, localBox.max.z},
        {localBox.max.x, localBox.min.y, localBox.max.z},
        {localBox.min.x, localBox.max.y, localBox.max.z},
        {localBox.max.x, localBox.max.y, localBox.max.z}
    };

    for (int i = 0; i < 8; ++i) {
        glm::vec4 transformed = transform * glm::vec4(corners[i], 1.0f);
        worldBox.grow(glm::vec3(transformed));
    }

    return worldBox;
}

inline bool hitAABB(const Ray& ray, const AABB& box, float& tResult) {
    float tmin = (box.min.x - ray.origin.x) / ray.direction.x;
    float tmax = (box.max.x - ray.origin.x) / ray.direction.x;

    if (tmin > tmax) std::swap(tmin, tmax);

    float tymin = (box.min.y - ray.origin.y) / ray.direction.y;
    float tymax = (box.max.y - ray.origin.y) / ray.direction.y;

    if (tymin > tymax) std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax)) return false;

    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;

    float tzmin = (box.min.z - ray.origin.z) / ray.direction.z;
    float tzmax = (box.max.z - ray.origin.z) / ray.direction.z;

    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax)) return false;

    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;

    if (tmin < 0.0f) {
        tmin = tmax;
        if (tmin < 0.0f) return false;
    }

    tResult = tmin;
    return true;
}

inline AABB calculateWorldAABB(const AABB& localAABB, const glm::vec3& position, const glm::vec3& scale) {
    AABB worldAABB;

    glm::vec3 corners[8] = {
        {localAABB.min.x, localAABB.min.y, localAABB.min.z},
        {localAABB.max.x, localAABB.min.y, localAABB.min.z},
        {localAABB.min.x, localAABB.max.y, localAABB.min.z},
        {localAABB.max.x, localAABB.max.y, localAABB.min.z},
        {localAABB.min.x, localAABB.min.y, localAABB.max.z},
        {localAABB.max.x, localAABB.min.y, localAABB.max.z},
        {localAABB.min.x, localAABB.max.y, localAABB.max.z},
        {localAABB.max.x, localAABB.max.y, localAABB.max.z}
    };

    for (int i = 0; i < 8; ++i) {
        glm::vec3 worldCorner = (corners[i] * scale) + position;
        worldAABB.grow(worldCorner);
    }

    return worldAABB;
}