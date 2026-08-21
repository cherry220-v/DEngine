#include "ComponentRegistry.hpp"
#include "../Scene.hpp"
#include "SerializationContext.hpp"

#include <assets/AssetManager.hpp>
#include <assets/IAssetRef.hpp>

#include "../components/IDComponent.hpp"

void ComponentRegistry::registerComponent(
    const ComponentInfo& component)
{
    m_components[component.name] =
        component;
}

ComponentInfo* ComponentRegistry::getComponent(
    const std::string& name)
{
    auto it = m_components.find(name);

    if (it == m_components.end())
        return nullptr;

    return &it->second;
}

std::unordered_map<std::string, ComponentInfo> ComponentRegistry::components()
{
    return m_components;
}

nlohmann::json serializeComponent(void* component, const ComponentInfo& info, const SerializationContext& ctx)
{
    nlohmann::json j;

    for (auto& prop : info.properties)
    {
        void* ptr =
            (char*)component + prop.offset;

        j[prop.name] =
            serializeProperty(
                ptr,
                prop.type,
                ctx
            );
    }

    return j;
}

void deserializeComponent(void* component, const ComponentInfo& info, const nlohmann::json& j, const SerializationContext& ctx)
{
    for (auto& prop : info.properties)
    {
        if (!j.contains(prop.name))
            continue;

        void* ptr =
            (char*)component + prop.offset;

        deserializeProperty(
            ptr,
            prop.type,
            j[prop.name],
            ctx
        );
    }
}

void finalizeComponentLinks(
    void* component,
    const ComponentInfo& info,
    const nlohmann::json& data,
    const SerializationContext& ctx)
{
    for (const auto& prop : info.properties)
    {
        if (!data.contains(prop.name))
            continue;

        const auto& value = data[prop.name];

        uint8_t* base = reinterpret_cast<uint8_t*>(component);

        switch (prop.type)
        {
        case PropertyType::Asset:
        {
            std::string uuid =
                value.get<std::string>();

            auto* assetRef =
                reinterpret_cast<IAssetRef*>(
                    base + prop.offset
                    );

            assetRef->setUuid(uuid);

            break;
        }

        case PropertyType::Entity:
        {
            std::string id = value.get<std::string>();

            auto it = ctx.entityMap.find(id);
            if (it != ctx.entityMap.end())
            {
                *reinterpret_cast<Entity*>(base + prop.offset)
                    = it->second;
            }

            break;
        }

        default:
            break;
        }
    }
}