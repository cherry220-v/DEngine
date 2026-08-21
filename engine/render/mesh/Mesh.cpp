#include "Mesh.hpp"

#include <glad/glad.h>
#include "../texture/Material.hpp"

#include <ecs/components/ModelRendererComponent.hpp>
#include <ecs/components/Transform.hpp>
#include <models/Model.hpp>

#include <iostream>

Mesh::Mesh(const std::vector<Vertex>& vertices)
{
    auto indices = autoIndex(vertices);
    setupMesh(vertices, indices);

}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
{
    setupMesh(vertices, indices);

}

Mesh::~Mesh()
{
    if (VBO != 0)
        glDeleteBuffers(1, &VBO);

    if (EBO != 0)
        glDeleteBuffers(1, &EBO);

    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
}

void Mesh::draw() const
{
    glBindVertexArray(VAO);

    glDrawElements(
        GL_TRIANGLES,
        (GLsizei)indexCount,
        GL_UNSIGNED_INT,
        nullptr
    );
}

void Mesh::setupMesh(
    const std::vector<Vertex>& vertices,
    const std::vector<uint32_t>& indices)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        EBO
    );

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(uint32_t),
        indices.data(),
        GL_STATIC_DRAW
    );

    this->vertices = vertices;
    this->indices = indices;

    vertexCount = this->vertices.size();
    indexCount = this->indices.size();

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, position)
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, normal)
    );
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, uv)
    );
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(
        3, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, barycentric)
    );
    glEnableVertexAttribArray(3);

    glVertexAttribIPointer(
        4, 4, GL_INT,
        sizeof(Vertex),
        (void*)offsetof(Vertex, boneIds)
    );
    glEnableVertexAttribArray(4);

    glVertexAttribPointer(
        5, 4, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, weights)
    );
    glEnableVertexAttribArray(5);

    glBindVertexArray(0);
}

std::vector<uint32_t> Mesh::autoIndex(
    const std::vector<Vertex>& input)
{
    return std::vector<uint32_t>();
}

AABB calculateModelWorldAABB(ModelRendererComponent& model, Transform& transform, const glm::mat4& entityWorldMatrix) {
    AABB totalWorldAABB;

    for (auto mesh : model.model.get()->meshes) {
        glm::mat4 meshWorldMatrix = entityWorldMatrix * transform.getMatrix();

        AABB meshWorldAABB = transformAABB(mesh->localAABB, meshWorldMatrix);

        totalWorldAABB.merge(meshWorldAABB);
    }

    return totalWorldAABB;
}