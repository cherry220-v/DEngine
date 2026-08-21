#pragma once

#include <ecs/entity/Entity.hpp>
#include <assets/IAsset.hpp>
#include <ecs/systems/ISystem.hpp>
#include <glm/mat4x4.hpp>

#include <core/GUUID.h>
#include "ComponentPool.hpp"

#include <unordered_map>
#include <typeindex>
#include <vector>
#include <memory>
#include <stdexcept>
#include "IComponentPool.hpp"

#include <nlohmann/json.hpp>
#include <iostream>
#include <editor/DrawTypes.h>
#include "SceneGraph.hpp"
#include "entity/EntityManager.hpp"

class Scene : public IAsset
{
public:
    SceneGraph& getSceneGraph()
    {
        return sceneGraph;
    }

    const SceneGraph& getSceneGraph() const
    {
        return sceneGraph;
    }

    EntityManager& getEntityManager()
    {
        return entityManager;
    }

    const EntityManager& getEntityManager() const
    {
        return entityManager;
    }
public:
    Entity createEntity(Entity parent = NullEntity, std::string name = "", std::string uuid="");
    void setParent(Entity e, Entity parent);
public:
    template<typename T>
    T& addComponent(Entity e, const T& component)
    {
        return getPool<T>().add(e, component);
    }

    template<typename T>
    T& addComponent(Entity e)
    {
        return getPool<T>().add(e, T{});
    }

    template<typename T>
    T* getComponent(Entity e)
    {
        return getPool<T>().get(e);
    }

    template<typename T>
    bool hasComponent(Entity e)
    {
        return getPool<T>().has(e);
    }

    template<typename T>
    void removeComponent(Entity e)
    {
        return getPool<T>().remove(e);
    }

    void* getComponentRaw(
        Entity e,
        const std::string& name
    );

    void* addComponentRaw(
        Entity e,
        const std::string& name
    );

    Entity pick(const Ray& ray);

    void selectEntity(Entity e);
    void deselectEntity(Entity e);
    void clearSelection();
    std::vector<Entity> getSelectedEntities();

    template<typename... Components>
    std::vector<Entity> view()
    {
        std::vector<Entity> result;

        auto& firstPool =
            getPool<
            typename std::tuple_element<
            0,
            std::tuple<Components...>
            >::type
            >();

        for (auto& [entity, _] : firstPool.data())
        {
            if ((getPool<Components>().has(entity) && ...))
                result.push_back(entity);
        }

        return result;
    }

    template<typename T>
    ComponentPool<T>& getPool()
    {
        auto type = std::type_index(typeid(T));

        auto it = m_componentPools.find(type);

        if (it == m_componentPools.end())
        {
            auto pool = std::make_shared<ComponentPool<T>>();

            m_componentPools[type] = pool;

            return *pool;
        }

        return *static_cast<ComponentPool<T>*>(
            it->second.get()
        );
    }

    glm::mat4 getWorldMatrix(Entity e);

    void setWorldMatrix(
        Entity e,
        const glm::mat4& world);

    const std::vector<std::unique_ptr<ISystem>>& systems() { return m_systems; };

    void initSystems(EngineContext* ctx);
    void updateSystems(Scene& scene, float dt);

    template<typename T>
    T* getSystem()
    {
        for (auto& s : m_systems)
        {
            if (auto ptr = dynamic_cast<T*>(s.get()))
                return ptr;
        }
        return nullptr;
    }

    nlohmann::json serialize();
    static std::unique_ptr<Scene> deserialize(const nlohmann::json& j);
private:
	EntityManager entityManager;
    SceneGraph sceneGraph;

    std::unordered_map<
        std::type_index,
        std::shared_ptr<IComponentPool>
    > m_componentPools;

    std::vector<std::unique_ptr<ISystem>> m_systems;
};