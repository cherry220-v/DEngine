#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 barycentric;

    glm::ivec4 boneIds{ 0 };
    glm::vec4 weights{ 0.0f };

    Vertex(
        glm::vec3 position,
        glm::vec3 normal,
        glm::vec2 uv,
        glm::vec3 barycentric,
        glm::ivec4 boneIds,
        glm::vec4 weights
    )
        : position(position)
        , normal(normal)
        , uv(uv)
        , barycentric(barycentric)
        , boneIds(boneIds)
        , weights(weights)
    {
    }

    Vertex()
    {
        position = {};
        normal = {};
        uv = {};
        barycentric = {};
        boneIds = { 0,0,0,0 };
        weights = { 0,0,0,0 };
    }

    bool operator==(const Vertex& o) const
    {
        return position == o.position &&
            normal == o.normal &&
            uv == o.uv;
    }
};

struct VertexHash
{
    size_t operator()(const Vertex& v) const
    {
        size_t h = 0;

        auto c = [&h](size_t x)
            {
                h ^= x + 0x9e3779b9 + (h << 6) + (h >> 2);
            };

        c(std::hash<float>{}(v.position.x));
        c(std::hash<float>{}(v.position.y));
        c(std::hash<float>{}(v.position.z));

        c(std::hash<float>{}(v.normal.x));
        c(std::hash<float>{}(v.normal.y));
        c(std::hash<float>{}(v.normal.z));

        c(std::hash<float>{}(v.uv.x));
        c(std::hash<float>{}(v.uv.y));

        return h;
    }
};

struct GizmoVertex
{
    glm::vec2 position;
    glm::vec4 color;
};