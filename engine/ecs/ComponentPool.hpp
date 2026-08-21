#pragma once

#include <unordered_map>
#include <utility>

#include "IComponentPool.hpp"

template<typename T>
class ComponentPool : public IComponentPool
{
public:
    using Storage = std::unordered_map<Entity, T>;

public:
    T& add(Entity entity, const T& component)
    {
        return m_components[entity] = component;
    }

    template<typename... Args>
    T& emplace(Entity entity, Args&&... args)
    {
        return m_components.emplace(
            entity,
            T(std::forward<Args>(args)...)
        ).first->second;
    }

    bool has(Entity entity) const
    {
        return m_components.find(entity) != m_components.end();
    }

    void remove(Entity entity) override
    {
        m_components.erase(entity);
    }

    T* get(Entity entity)
    {
        auto it = m_components.find(entity);

        if (it == m_components.end())
            return nullptr;

        return &it->second;
    }

    Storage& data()
    {
        return m_components;
    }

    const Storage& data() const
    {
        return m_components;
    }

private:
    Storage m_components;
};