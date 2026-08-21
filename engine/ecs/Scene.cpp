#include "Scene.hpp"

#include "registry/SerializationContext.hpp"
#include <core/EngineContext.hpp>
#include <ecs/registry/SystemRegistry.hpp>

#include <ecs/registry/ComponentRegistry.hpp>

#include <ecs/components/ModelRendererComponent.hpp>
#include <ecs/components/Transform.hpp>
#include <ecs/components/CameraComponent.hpp>
#include <ecs/components/IDComponent.hpp>
#include <core/GUUID.h>

#include <stdexcept>
#include <iostream>
#include "components/SelectedComponent.hpp"
#include "components/HierarchyComponent.hpp"
#include "components/NameComponent.hpp"

#include <QDebug>

Entity Scene::createEntity(Entity parent, std::string name, std::string uuid)
{
    Entity e = entityManager.createEntity();

    auto& compId = addComponent<IDComponent>(e);
    auto& compHierarchy = addComponent<HierarchyComponent>(e);
    auto& compName = addComponent<NameComponent>(e);
    compId.id = uuid.empty() ? GUUID::generate() : uuid;
    compName.name = name;

    compHierarchy.parent = parent;
    if (parent != NullEntity)
    {
        compHierarchy.children.push_back(e);
    }

    sceneGraph.addEntity(e);

    return e;
}

void Scene::setParent(Entity e, Entity newParent)
{
    auto* h = getComponent<HierarchyComponent>(e);

    if (h->parent != NullEntity)
    {
        auto* oldParent = getComponent<HierarchyComponent>(h->parent);
        erase(oldParent->children, e);
    }

    h->parent = newParent;

    if (newParent != NullEntity)
    {
        auto* np = getComponent<HierarchyComponent>(newParent);
        np->children.push_back(e);
    }
    sceneGraph.setParent(e, newParent);
}

void* Scene::getComponentRaw(Entity e, const std::string& name)
{
    auto& registry = ComponentRegistry::get();
    auto components = registry.components();

    auto it = components.find(name);
    if (it == components.end())
        return nullptr;

    const ComponentInfo& info = it->second;

    if (!info.hasComponent(this, e))
        return nullptr;

    return info.getComponent(this, e);
}

void* Scene::addComponentRaw(Entity e, const std::string& name)
{
    auto& registry = ComponentRegistry::get();
    auto components = registry.components();

    auto it = components.find(name);
    if (it == components.end())
        return nullptr;

    const ComponentInfo& info = it->second;

    return info.addComponent(this, e);
}

Entity Scene::pick(const Ray& ray)
{
//    Entity result = NullEntity;
//    float closestDistance = std::numeric_limits<float>::max();
//
//    for (Entity entity : entityManager.aliveEntities())
//    {
//        auto* transform = getComponent<Transform>(entity);
//        auto* model = getComponent<ModelRendererComponent>(entity);
//
//        if (!transform || !model)
//            continue;
//
//        AABB localBounds = model->model.get()->bounds;
//
//        AABB worldBounds =
//            TransformAABB(localBounds,
//                transform->getMatrix());
//
//        float distance;
//
//        bool hit = IntersectRayAABB(
//            ray,
//            worldBounds,
//            distance);
//
//        if (hit)
//        {
//            if (distance < closestDistance)
//            {
//                closestDistance = distance;
//                result = entity;
//            }
//        }
//    }
//
//std::cout << "Picked Entity ID: " << result << std::endl;
//return result;
    return 0;
}


void Scene::selectEntity(Entity e)
{
    addComponent<SelectedComponent>(e);
}

void Scene::deselectEntity(Entity e)
{
    removeComponent<SelectedComponent>(e);
}

void Scene::clearSelection()
{
    for (auto& e : view<SelectedComponent>())
    {
        removeComponent<SelectedComponent>(e);
    }
}

std::vector<Entity> Scene::getSelectedEntities()
{
    return view<SelectedComponent>();
}

void Scene::initSystems(EngineContext* ctx)
{
    auto& registry = SystemRegistry::get();

    const auto& factories = registry.factories();

    for (auto& factory : factories)
    {
        auto sys = factory();
        if (sys)
            m_systems.push_back(std::move(sys));
    }

    for (auto& sys : m_systems)
    {
        if (sys)
            sys->init(ctx);
    }
}

void Scene::updateSystems(Scene& scene, float dt)
{
    for (auto& sys : m_systems)
        sys->update(scene, dt);
}

glm::mat4 Scene::getWorldMatrix(Entity e)
{
    auto* transform =
        getComponent<Transform>(e);

    if (!transform)
        return glm::mat4(1.0f);

    Entity parent =
        sceneGraph.getParent(e);

    if (parent == NullEntity)
    {
        return transform->getMatrix();
    }

    return
        getWorldMatrix(parent) *
        transform->getMatrix();
}

void Scene::setWorldMatrix(
    Entity e,
    const glm::mat4& world)
{
    auto* transform =
        getComponent<Transform>(e);
    if (!transform)
        return;
    Entity parent =
        sceneGraph.getParent(e);
    if (parent == NullEntity)
    {
        transform->updateFromMatrix(world);
        return;
    }
    glm::mat4 parentWorld =
        getWorldMatrix(parent);
    glm::mat4 local = glm::inverse(parentWorld) * world;
    transform->updateFromMatrix(local);
}

nlohmann::json Scene::serialize()
{
    SerializationContext ctx;
    ctx.scene = this;
    nlohmann::json sceneJson;

    for (auto entity : entityManager.aliveEntities())
    {
        nlohmann::json entityJson;

        auto* comp = getComponent<IDComponent>(entity);
        if (!comp)
        {
            qDebug() << "NO ID COMPONENT FOR ENTITY" << entity;
            continue;
        }

        entityJson["id"] = comp->id;

        nlohmann::json componentsJson;

        for (auto& [name, info] :
            ComponentRegistry::get().components())
        {
            if (!info.hasComponent(this, entity))
                continue;

            void* component =
                info.getComponent(this, entity);

            componentsJson[name] =
                serializeComponent(
                    component,
                    info,
                    ctx
                );
        }

        entityJson["components"] =
            componentsJson;

        sceneJson["entities"]
            .push_back(entityJson);
    }

    return sceneJson;
}

std::unique_ptr<Scene> Scene::deserialize(const nlohmann::json& j)
{
    auto scene = std::make_unique<Scene>();
    std::unordered_map<std::string, Entity> entityMap;
    if (j.contains("entities"))
    {
        for (const auto& entityJson : j["entities"])
        {
            std::string id = entityJson.at("id").get<std::string>();
            Entity e = scene->createEntity();
            auto* comp = scene->getComponent<IDComponent>(e);
            if (!comp)
                comp = &scene->addComponent<IDComponent>(e);
            comp->id = id;
            entityMap[id] = e;
        }
    }

    for (const auto& entityJson : j["entities"])
    {
        std::string id = entityJson.at("id").get<std::string>();
        Entity e = entityMap.at(id);

        if (!entityJson.contains("components"))
            continue;

        const auto& componentsJson = entityJson["components"];

        for (auto& [name, info] : ComponentRegistry::get().components())
        {
            if (!componentsJson.contains(name))
                continue;

            void* componentPtr = info.addComponent(scene.get(), e);
            deserializeComponent(componentPtr, info, componentsJson[name], SerializationContext{ scene.get(), {} });
        }
    }

    for (const auto& entityJson : j["entities"])
    {
        std::string id = entityJson.at("id").get<std::string>();
        Entity e = entityMap.at(id);

        const auto& componentsJson = entityJson.value("components", nlohmann::json::object());

        for (auto& [name, info] : ComponentRegistry::get().components())
        {
            if (!componentsJson.contains(name))
                continue;

            void* componentPtr = info.getComponent(scene.get(), e);
            finalizeComponentLinks(componentPtr, info, componentsJson[name], SerializationContext{ scene.get(), entityMap });
        }
    }

    return scene;
}