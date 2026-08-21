#include "GizmoContext.hpp"

using namespace OverlayMath;

namespace Gizmo
{
    // ----------------------------
    // Axis helpers
    // ----------------------------

    inline glm::vec3 AxisDir(GizmoAxis axis, const glm::mat4& view)
    {
        switch (axis)
        {
        case GizmoAxis::X: return { 1,0,0 };
        case GizmoAxis::Y: return { 0,1,0 };
        case GizmoAxis::Z: return { 0,0,1 };

        default: return { 1,0,0 };
        }
    }

    // ----------------------------
    // Hit test (simple screen-space axis picking)
    // ----------------------------

    inline GizmoAxis HitTestTranslate(
        const glm::vec2& mouse,
        const GizmoContext& ctx,
        const ScreenParams& sp,
        float screenSize = 80.0f)
    {
        glm::vec2 origin = WorldToScreen(ctx.origin, sp);

        glm::vec2 x = WorldToScreen(ctx.origin + glm::vec3(1, 0, 0) * 1.0f, sp);
        glm::vec2 y = WorldToScreen(ctx.origin + glm::vec3(0, 1, 0) * 1.0f, sp);
        glm::vec2 z = WorldToScreen(ctx.origin + glm::vec3(0, 0, 1) * 1.0f, sp);

        float dx = DistancePointToLine2D(mouse, origin, x);
        float dy = DistancePointToLine2D(mouse, origin, y);
        float dz = DistancePointToLine2D(mouse, origin, z);

        float minDist = screenSize;

        GizmoAxis axis = GizmoAxis::None;

        if (dx < minDist) { minDist = dx; axis = GizmoAxis::X; }
        if (dy < minDist) { minDist = dy; axis = GizmoAxis::Y; }
        if (dz < minDist) { minDist = dz; axis = GizmoAxis::Z; }

        return axis;
    }

    // ----------------------------
    // Translate solve (core ImGuizmo-like logic)
    // ----------------------------

    inline glm::vec3 SolveTranslate(
        const OverlayMath::Ray& ray,
        const GizmoContext& ctx,
        GizmoAxis axis,
        const ScreenParams& sp)
    {
        glm::vec3 dir;

        switch (axis)
        {
        case GizmoAxis::X: dir = { 1,0,0 }; break;
        case GizmoAxis::Y: dir = { 0,1,0 }; break;
        case GizmoAxis::Z: dir = { 0,0,1 }; break;
        default: return ctx.origin;
        }

        // Camera-facing plane trick (stable dragging)
        glm::vec3 camForward = OverlayMath::CameraForward(sp.view);

        glm::vec3 planeNormal = glm::cross(dir, camForward);
        if (glm::length(planeNormal) < 1e-5f)
            planeNormal = glm::cross(dir, glm::vec3(0, 1, 0));

        Plane plane;
        plane.point = ctx.origin;
        plane.normal = glm::normalize(planeNormal);

        glm::vec3 hit = RayPlanePoint(ray, plane);

        glm::vec3 projected = ProjectOnAxis(hit, ctx.origin, dir);

        return projected;
    }

    // ----------------------------
    // Main update
    // ----------------------------

    inline void Update(
        GizmoContext& ctx,
        const glm::vec2& mouse,
        const ScreenParams& sp,
        bool mouseDown,
        bool mouseUp)
    {
        Ray ray = ScreenPointToRay(mouse, sp);

        // -------------------
        // Idle -> Hover
        // -------------------

        if (ctx.state == GizmoState::Idle)
        {
            GizmoAxis hover = HitTestTranslate(mouse, ctx, sp);

            if (hover != GizmoAxis::None)
                ctx.activeAxis = hover;

            if (mouseDown && ctx.activeAxis != GizmoAxis::None)
            {
                ctx.state = GizmoState::Dragging;
                ctx.dragStartWorld = ctx.origin;
                ctx.lastWorld = ctx.origin;
                ctx.dragging = true;
            }
        }

        // -------------------
        // Dragging
        // -------------------

        if (ctx.state == GizmoState::Dragging)
        {
            glm::vec3 newPos = SolveTranslate(ray, ctx, ctx.activeAxis, sp);

            glm::vec3 delta = newPos - ctx.lastWorld;

            ctx.origin += delta;
            ctx.lastWorld = newPos;

            if (mouseUp)
            {
                ctx.state = GizmoState::Idle;
                ctx.activeAxis = GizmoAxis::None;
                ctx.dragging = false;
            }
        }
    }
}