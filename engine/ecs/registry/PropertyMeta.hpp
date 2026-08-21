#pragma once

#include <nlohmann/json.hpp>
#include <string>

struct SerializationContext;

enum class PropertyType
{
    Int,
    Float,
    Bool,
    Vec2,
    Vec3,
    Vec4,
    String,
    Entity,
    Asset,
    Enum
};

struct PropertyMeta
{
    std::string name;

    size_t offset;

    PropertyType type;
};

nlohmann::json serializeProperty(
    void* data,
    PropertyType type,
    const SerializationContext& ctx
);

void deserializeProperty(
    void* data,
    PropertyType type,
    const nlohmann::json& json,
    const SerializationContext& ctx);