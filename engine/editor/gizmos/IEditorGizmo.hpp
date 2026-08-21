#pragma once

#include <ecs/entity/Entity.hpp>
#include <ecs/components/CameraComponent.hpp>

class Scene;
struct EditorContext;

class IEditorGizmo
{
public:
    virtual ~IEditorGizmo() = default;

    virtual bool canDraw(
        Scene* scene,
        Entity e
    ) = 0;

    virtual void draw(
        Scene* scene,
        Entity e,
        EditorContext& ctx
    ) = 0;
};