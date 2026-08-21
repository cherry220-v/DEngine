#include "OBJLoader.hpp"

#include <tiny_obj_loader.h>
#include <stdexcept>

#include "../Model.hpp"
#include <render/mesh/Mesh.hpp>

#include <math/Vertex.h>
#include <math/VertexKey.h>

std::shared_ptr<IAsset> OBJLoader::load(const std::string& path)
{
    std::vector<std::shared_ptr<Mesh>> meshes;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn, err;

    bool success = tinyobj::LoadObj(
        &attrib,
        &shapes,
        &materials,
        &warn,
        &err,
        path.c_str(),
        nullptr,
        true
    );

    if (!success)
    {
        throw std::runtime_error("Failed to load OBJ: " + err);
    }
    for (const auto& shape : shapes)
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        std::unordered_map<VertexKey, uint32_t, VertexKeyHash>
            vertexMap;

        for (size_t i = 0; i < shape.mesh.indices.size(); i++)
        {
            const auto& idx =
                shape.mesh.indices[i];

            VertexKey key
            {
                idx.vertex_index,
                idx.normal_index,
                idx.texcoord_index
            };

            auto it = vertexMap.find(key);

            if (it != vertexMap.end())
            {
                indices.push_back(it->second);
                continue;
            }

            Vertex v;

            // POSITION
            v.position = {
                attrib.vertices[3 * idx.vertex_index + 0],
                attrib.vertices[3 * idx.vertex_index + 1],
                attrib.vertices[3 * idx.vertex_index + 2]
            };

            // NORMAL
            if (idx.normal_index >= 0)
            {
                v.normal = {
                    attrib.normals[3 * idx.normal_index + 0],
                    attrib.normals[3 * idx.normal_index + 1],
                    attrib.normals[3 * idx.normal_index + 2]
                };
            }

            // UV
            if (idx.texcoord_index >= 0)
            {
                v.uv = {
                    attrib.texcoords[2 * idx.texcoord_index + 0],
                    attrib.texcoords[2 * idx.texcoord_index + 1]
                };
            }

            uint32_t newIndex =
                static_cast<uint32_t>(
                    vertices.size()
                    );

            vertices.push_back(v);

            indices.push_back(newIndex);

            vertexMap[key] = newIndex;
        }

        meshes.push_back(
            std::make_shared<Mesh>(
                vertices,
                indices
            )
        );
    }
    auto model = std::make_shared<Model>(meshes);

    return model;
}
