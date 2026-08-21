#pragma once

struct VertexKey
{
    int vertex;
    int normal;
    int texcoord;

    bool operator==(const VertexKey&) const = default;
};

struct VertexKeyHash
{
    size_t operator()(const VertexKey& k) const
    {
        size_t h = 0;

        h ^= std::hash<int>{}(k.vertex);
        h ^= std::hash<int>{}(k.normal) << 1;
        h ^= std::hash<int>{}(k.texcoord) << 2;

        return h;
    }
};