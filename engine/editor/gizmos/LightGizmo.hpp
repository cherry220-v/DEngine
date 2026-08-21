#pragma once

#include "IEditorGizmo.hpp"
#include <ecs/components/LightComponent.hpp>

class LightGizmo : public IEditorGizmo
{
public:
    bool canDraw(
        Scene* scene,
        Entity e
    ) override
    {
        return scene->hasComponent<LightComponent>(e);
    }

    void draw(
        Scene* scene,
        Entity e,
        const CameraComponent& camera
    ) override;
};