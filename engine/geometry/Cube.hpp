#pragma once

#include <models/Model.hpp>
#include <render/mesh/Mesh.hpp>
#include <ecs/components/ModelRendererComponent.hpp>
#include <ecs/components/IComponent.hpp>
#include <render/texture/Material.hpp>

class CubeMesh : public Mesh
{
public:
    CubeMesh()
        : Mesh(getVertices(), getIndices())
    {
    }

private:

    std::vector<Vertex> getVertices()
    {
        std::vector<Vertex> v;
        v.reserve(24);

        // FRONT (+Z)
        addFace(v,
            { -0.5f,-0.5f, 0.5f },
            { 0.5f,-0.5f, 0.5f },
            { 0.5f, 0.5f, 0.5f },
            { -0.5f, 0.5f, 0.5f },
            { 0,0,1 });

        // BACK (-Z)
        addFace(v,
            { 0.5f,-0.5f,-0.5f },
            { -0.5f,-0.5f,-0.5f },
            { -0.5f, 0.5f,-0.5f },
            { 0.5f, 0.5f,-0.5f },
            { 0,0,-1 });

        // LEFT (-X)
        addFace(v,
            { -0.5f,-0.5f,-0.5f },
            { -0.5f,-0.5f, 0.5f },
            { -0.5f, 0.5f, 0.5f },
            { -0.5f, 0.5f,-0.5f },
            { -1,0,0 });

        // RIGHT (+X)
        addFace(v,
            { 0.5f,-0.5f, 0.5f },
            { 0.5f,-0.5f,-0.5f },
            { 0.5f, 0.5f,-0.5f },
            { 0.5f, 0.5f, 0.5f },
            { 1,0,0 });

        // TOP (+Y)
        addFace(v,
            { -0.5f, 0.5f, 0.5f },
            { 0.5f, 0.5f, 0.5f },
            { 0.5f, 0.5f,-0.5f },
            { -0.5f, 0.5f,-0.5f },
            { 0,1,0 });

        // BOTTOM (-Y)
        addFace(v,
            { -0.5f,-0.5f,-0.5f },
            { 0.5f,-0.5f,-0.5f },
            { 0.5f,-0.5f, 0.5f },
            { -0.5f,-0.5f, 0.5f },
            { 0,-1,0 });

        return v;
    }

    std::vector<uint32_t> getIndices()
    {
        std::vector<uint32_t> i;
        i.reserve(36);

        for (uint32_t face = 0; face < 6; face++)
        {
            uint32_t base = face * 4;

            i.push_back(base + 0);
            i.push_back(base + 1);
            i.push_back(base + 2);

            i.push_back(base + 2);
            i.push_back(base + 3);
            i.push_back(base + 0);
        }

        return i;
    }

    void addFace(
        std::vector<Vertex>& v,
        glm::vec3 a, glm::vec3 b,
        glm::vec3 c, glm::vec3 d,
        glm::vec3 normal)
    {
        Vertex vertex;
        vertex.position = a;
        vertex.normal = normal;
        vertex.uv = { 0, 0 };
        vertex.barycentric = { 1,0,0 };

        Vertex vertex2;
        vertex2.position = b;
        vertex2.normal = normal;
        vertex2.uv = { 1, 0 };
        vertex2.barycentric = { 0,1,0 };

        Vertex vertex3;
        vertex3.position = c;
        vertex3.normal = normal;
        vertex3.uv = { 1, 1 };
        vertex3.barycentric = { 0,0,1 };

        Vertex vertex4;
        vertex4.position = d;
        vertex4.normal = normal;
        vertex4.uv = { 0, 1 };
        vertex4.barycentric = { 1,0,0 };

        v.push_back(vertex);
        v.push_back(vertex2);
        v.push_back(vertex3);
        v.push_back(vertex4);
    }
};

class CubeModel : public Model
{
public:
    CubeModel() { meshes.push_back({ std::make_shared<CubeMesh>() }); }
    //void load(const std::string& path) override {}
};