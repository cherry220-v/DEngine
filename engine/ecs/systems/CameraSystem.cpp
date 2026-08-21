#include "CameraSystem.hpp"

#include <core/EngineContext.hpp>

#include <ecs/components/CameraComponent.hpp>
#include <ecs/components/Transform.hpp>

#include <ecs/Scene.hpp>
#include <math/CameraMath.h>

void CameraSystem::init(EngineContext* context) {}

void CameraSystem::update(Scene& scene, float dt)
{
    if (activeCamera == 0)
        return;

    Transform* transform = scene.getComponent<Transform>(activeCamera);
    CameraComponent* cam = scene.getComponent<CameraComponent>(activeCamera);

    //cam->viewMatrix = getViewMatrix(*transform);
    //cam->projectionMatrix = getProjectionMatrix(*cam);
}