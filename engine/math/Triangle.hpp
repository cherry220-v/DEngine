#pragma once

#include <vector>
#include <glm/glm.hpp>

#include <editor/DrawTypes.h>

struct Triangle {
    glm::vec3 v0, v1, v2;
};

struct MeshGeometry {
    AABB localAABB;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    size_t getTriangleCount() const {
        if (indices.empty()) {
            return vertices.size() / 3;
        }
        return indices.size() / 3;
    }

    Triangle getTriangle(size_t triangleIndex) const {
        Triangle tri;
        if (!indices.empty()) {
            size_t idx0 = indices[triangleIndex * 3 + 0];
            size_t idx1 = indices[triangleIndex * 3 + 1];
            size_t idx2 = indices[triangleIndex * 3 + 2];

            tri.v0 = vertices[idx0].position;
            tri.v1 = vertices[idx1].position;
            tri.v2 = vertices[idx2].position;
        }
        else {
            tri.v0 = vertices[triangleIndex * 3 + 0].position;
            tri.v1 = vertices[triangleIndex * 3 + 1].position;
            tri.v2 = vertices[triangleIndex * 3 + 2].position;
        }
        return tri;
    }
};

inline MeshGeometry loadMeshFromRawData(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
    MeshGeometry mesh;
    mesh.vertices = vertices;
    mesh.indices = indices;

    for (const auto& vertex : vertices) {
        mesh.localAABB.grow(vertex.position);
    }

    return mesh;
}

inline bool hitTriangle(const Ray& ray, const Triangle& tri, float& t, float& u, float& v) {
    const float EPSILON = 0.000001f;
    glm::vec3 edge1 = tri.v1 - tri.v0;
    glm::vec3 edge2 = tri.v2 - tri.v0;

    glm::vec3 h = glm::cross(ray.direction, edge2);
    float a = glm::dot(edge1, h);

    if (a > -EPSILON && a < EPSILON) return false; // Луч параллелен треугольнику

    float f = 1.0f / a;
    glm::vec3 s = ray.origin - tri.v0;
    u = f * glm::dot(s, h);

    if (u < 0.0f || u > 1.0f) return false;

    glm::vec3 q = glm::cross(s, edge1);
    v = f * glm::dot(ray.direction, q);

    if (v < 0.0f || u + v > 1.0f) return false;

    t = f * glm::dot(edge2, q);

    return t > EPSILON; // Пересечение найдено
}