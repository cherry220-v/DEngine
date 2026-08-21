#pragma once

#include <vector>
#include <math/Vertex.h>
#include <math/VertexKey.h>
#include <unordered_map>
#include <glm/vec3.hpp>

#include "AABB.hpp"
#include <math/Triangle.hpp>

struct ModelRendererComponent;
struct Transform;

struct IndexedMesh
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

class Mesh
{
public:
    Mesh(const std::vector<Vertex>& vertices);
    Mesh(const std::vector<Vertex>& vertices,
         const std::vector<uint32_t>& indices);
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    ~Mesh();

    void draw() const;

    const std::vector<Vertex>& getVertices() const { return vertices; }
    const std::vector<uint32_t>& getIndices() const { return indices; }

    size_t getIndexCount() { return indexCount; }
    size_t getVertexCount() { return vertexCount; }

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
    AABB localAABB;

private:
    void setupMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

    std::vector<uint32_t> autoIndex(
        const std::vector<Vertex>& vertices);

private:
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

    size_t indexCount = 0;
    size_t vertexCount = 0;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

AABB calculateModelWorldAABB(ModelRendererComponent& model, Transform& transform, const glm::mat4& entityWorldMatrix);