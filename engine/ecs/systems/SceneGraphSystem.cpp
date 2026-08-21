#include "SceneGraphSystem.hpp"

#include <ecs/components/Transform.hpp>
#include <iostream>

void SceneGraphSystem::init(EngineContext* context)
{
}

void SceneGraphSystem::update(
    Scene& scene,
    float dt)
{
    auto& graph =
		scene.getSceneGraph();
    for (Entity root : graph.getRoots())
    {
        updateNode(
            &scene,
            root,
            glm::mat4(1.0f)
        );
    }
}

void SceneGraphSystem::updateNode(
    Scene* scene,
    Entity entity,
    const glm::mat4& parentWorld)
{
    auto & graph =
		scene->getSceneGraph();
    auto* transform =
        scene->getComponent<Transform>(entity);

    glm::mat4 world =
        parentWorld;

    if (transform)
    {
        transform->worldMatrix =
            parentWorld *
            transform->getMatrix();

        world =
            transform->worldMatrix;
    }

    for (Entity child : graph.getChildren(entity))
    {
        updateNode(
            scene,
            child,
            world
        );
    }
}