#include "Model.hpp"

#include <render/mesh/Mesh.hpp>
#include <render/texture/Material.hpp>
#include <core/prints.h>

void Model::draw()
{
    for (auto& mesh : meshes)
    {
        mesh->draw();
    }
}

void Model::buildPhysicsCache(const std::vector<IndexedMesh>& rawMeshes)
{
    cachedVertices.clear();
    cachedIndices.clear();

    uint32_t vertexOffset = 0;
    for (const auto& rawMesh : rawMeshes)
    {
        cachedVertices.insert(cachedVertices.end(), rawMesh.vertices.begin(), rawMesh.vertices.end());

        for (uint32_t index : rawMesh.indices)
        {
            cachedIndices.push_back(index + vertexOffset);
        }

        vertexOffset += static_cast<uint32_t>(rawMesh.vertices.size());
    }
}

uint32_t Model::getVertexCount() const
{
    uint32_t sz = 0;
    for (const auto& mesh : meshes)
    {
        if (mesh) sz += static_cast<uint32_t>(mesh->getVertexCount());
    }
    return sz;
}

uint32_t Model::getVertexStride() const
{
    return static_cast<uint32_t>(sizeof(Vertex));
}

const void* Model::getVertexData() const
{
    auto* nonConstThis = const_cast<Model*>(this);
    nonConstThis->cachedVertices.clear();
    nonConstThis->cachedVertices.reserve(getVertexCount());

    for (const auto& mesh : meshes)
    {
        if (!mesh) continue;
        const auto& meshVertices = mesh->getVertices();
        nonConstThis->cachedVertices.insert(nonConstThis->cachedVertices.end(), meshVertices.begin(), meshVertices.end());
    }

    return cachedVertices.data();
}

uint32_t Model::getIndexCount() const
{
    uint32_t sz = 0;
    for (const auto& mesh : meshes)
    {
        if (mesh) sz += static_cast<uint32_t>(mesh->getIndexCount());
    }
    return sz;
}

uint32_t Model::getIndexStride() const
{
    return static_cast<uint32_t>(sizeof(uint32_t));
}

const void* Model::getIndexData() const
{
    auto* nonConstThis = const_cast<Model*>(this);
    nonConstThis->cachedIndices.clear();
    nonConstThis->cachedIndices.reserve(getIndexCount());

    uint32_t vertexOffset = 0;
    for (const auto& mesh : meshes)
    {
        if (!mesh) continue;

        const auto& meshIndices = mesh->getIndices();
        for (uint32_t index : meshIndices)
        {
            nonConstThis->cachedIndices.push_back(index + vertexOffset);
        }

        vertexOffset += static_cast<uint32_t>(mesh->getVertexCount());
    }

    return cachedIndices.data();
}

void Model::initializeModelAABB(Model& model)
{
    model.localAABB = AABB();

    for (auto& mesh : model.meshes) {
        mesh->localAABB = AABB();

        for (const auto& vertex : mesh->getVertices()) {
            mesh->localAABB.grow(vertex.position);
        }

        model.localAABB.merge(mesh->localAABB);
    }
    std::cout << model.localAABB.min << std::endl;
    std::cout << model.localAABB.max << std::endl;
}