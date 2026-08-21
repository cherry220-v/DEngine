#pragma once
#include <glm/glm.hpp>
#include "GizmoTypes.hpp"
#include "OverlayMath.hpp"

struct GizmoContext
{
    GizmoMode mode = GizmoMode::Translate;
    GizmoAxis activeAxis = GizmoAxis::None;
    GizmoState state = GizmoState::Idle;

    glm::vec3 origin = { 0,0,0 };

    glm::vec3 dragStartWorld = { 0,0,0 };
    glm::vec3 lastWorld = { 0,0,0 };

    bool dragging = false;
};

namespace Gizmo
{
    inline void Update(
        GizmoContext& ctx,
        const glm::vec2& mouse,
        const OverlayMath::ScreenParams& sp,
        bool mouseDown,
        bool mouseUp);
};