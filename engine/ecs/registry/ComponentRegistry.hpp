#pragma once

#include "../components/IComponent.hpp"
#include "../entity/Entity.hpp"

#include "ComponentInfo.hpp"

#include <nlohmann/json.hpp>
#include <unordered_map>

#define REGISTER_COMPONENT(info) \
    static bool _registered_##info = []() \
    { \
        ComponentRegistry::get().registerComponent(info); \
        return true; \
    }()

nlohmann::json serializeComponent(
    void* component,
    const ComponentInfo& info, const SerializationContext& ctx);

void deserializeComponent(
    void* component,
    const ComponentInfo& info,
    const nlohmann::json& j, const SerializationContext& ctx);

void finalizeComponentLinks(
    void* component,
    const ComponentInfo& info,
    const nlohmann::json& data,
    const SerializationContext& ctx);

class ComponentRegistry
{
public:
    static ComponentRegistry& get() { static ComponentRegistry s; return s;}

public:
    void registerComponent(const ComponentInfo& component);

    ComponentInfo* getComponent(const std::string& name);

    std::unordered_map<std::string, ComponentInfo> components();

private:
    ComponentRegistry() = default;
    ~ComponentRegistry() = default;

    ComponentRegistry(
        const ComponentRegistry&
    ) = delete;

    ComponentRegistry& operator=(
        const ComponentRegistry&
        ) = delete;

private:
    std::unordered_map<std::string, ComponentInfo> m_components;
};
