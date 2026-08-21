#pragma once

#include "IEditorGizmo.hpp"
#include <ecs/components/Transform.hpp>
#include <ecs/registry/GizmoRegistry.hpp>

#include "../GizmoAxis.hpp"
#include <glm/vec3.hpp>

class TransformGizmo : public IEditorGizmo
{
public:
    bool canDraw(
        Scene* scene,
        Entity e
    ) override
    {
        return scene->hasComponent<Transform>(e);
    }

    void draw(
        Scene* scene,
        Entity e,
        EditorContext& ctx
    ) override;

private:
    GizmoAxis m_activeAxis =
        GizmoAxis::None;

    glm::vec3 m_dragStartPosition;

    glm::vec3 m_dragStartHit;
};

REGISTER_GIZMO(transformInfo.name, std::make_unique<TransformGizmo>());