#include "PropertyMeta.hpp"

#include <assets/AssetManager.hpp>
#include <assets/IAssetRef.hpp>

#include <ecs/Scene.hpp>
#include "SerializationContext.hpp"

#include <ecs/components/IDComponent.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <nlohmann/json.hpp>

nlohmann::json serializeProperty(
    void* data,
    PropertyType type,
    const SerializationContext& ctx
)
{
    switch (type)
    {
    case PropertyType::Int:
        return *(int*)data;

    case PropertyType::Float:
        return *(float*)data;

    case PropertyType::Bool:
        return *(bool*)data;

    case PropertyType::Vec2:
    {
        auto& v = *(glm::vec2*)data;

        return { v.x, v.y };
    }

    case PropertyType::Vec3:
    {
        auto& v = *(glm::vec3*)data;

        return { v.x, v.y, v.z };
    }

    case PropertyType::Vec4:
    {
        auto& v = *(glm::vec4*)data;

        return { v.x, v.y, v.z, v.w };
    }

    case PropertyType::String:
        return *(std::string*)data;

    case PropertyType::Enum:
        return *(int*)data;
    case PropertyType::Entity: {
        if (!ctx.scene) return "";
        Entity e = *(Entity*)data;
        auto* idComp = ctx.scene->getComponent<IDComponent>(e);
        if (!idComp) return "";
        return idComp->id;
    }
    case PropertyType::Asset:
    {
        auto assetRef = reinterpret_cast<IAssetRef*>(data);
        return assetRef->uuid();
    }
    }
    return {};
}

void deserializeProperty(
    void* data,
    PropertyType type,
    const nlohmann::json& json,
    const SerializationContext& ctx)
{
    switch (type)
    {
    case PropertyType::Int:
        *(int*)data =
            json.get<int>();
        break;

    case PropertyType::Float:
        *(float*)data =
            json.get<float>();
        break;

    case PropertyType::Bool:
        *(bool*)data =
            json.get<bool>();
        break;

    case PropertyType::Vec3:
    {
        auto& v =
            *(glm::vec3*)data;

        v.x = json[0];
        v.y = json[1];
        v.z = json[2];

        break;
    }

    case PropertyType::String:
    {
        *(std::string*)data =
            json.get<std::string>();

        break;
    }

    case PropertyType::Enum:
        *(int*)data =
            json.get<int>();
        break;

    }
}