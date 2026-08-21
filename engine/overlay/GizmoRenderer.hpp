#pragma once
#include "GizmoContext.hpp"
#include "OverlayMath.hpp"
#include <editor/DrawList.hpp>

struct DebugDrawAPI
{
    virtual void Line(glm::vec3 a, glm::vec3 b, glm::vec3 color) = 0;
};

namespace GizmoRender
{
    inline void DrawAxis(DebugDrawAPI* dd, const GizmoContext& ctx)
    {
        dd->Line(ctx.origin, ctx.origin + glm::vec3(1, 0, 0), { 1,0,0 });
        dd->Line(ctx.origin, ctx.origin + glm::vec3(0, 1, 0), { 0,1,0 });
        dd->Line(ctx.origin, ctx.origin + glm::vec3(0, 0, 1), { 0,0,1 });
    }

    inline bool Project(
        const glm::vec3& p,
        const glm::mat4& vp,
        const glm::vec2& viewport,
        glm::vec2& out)
    {
        glm::vec4 clip = vp * glm::vec4(p, 1.0f);

        if (clip.w <= 0.0f)
            return false;

        glm::vec3 ndc = glm::vec3(clip) / clip.w;

        out.x = (ndc.x * 0.5f + 0.5f) * viewport.x;
        out.y = (1.0f - (ndc.y * 0.5f + 0.5f)) * viewport.y;

        return true;
    }

    inline void DrawTranslateGizmo(
        DrawList& dl,
        const glm::vec3& pos,
        const glm::mat4& view,
        const glm::mat4& proj,
        const glm::vec2& viewport)
    {
        glm::vec3 camPos = glm::vec3(glm::inverse(view)[3]);

        float dist = glm::distance(camPos, pos);

        float size = dist * 0.15f;

        glm::vec3 px = pos + glm::vec3(size, 0, 0);
        glm::vec3 py = pos + glm::vec3(0, size, 0);
        glm::vec3 pz = pos + glm::vec3(0, 0, size);

        glm::mat4 vp = proj * view;

        glm::vec2 s0, sx, sy, sz;

        if (!Project(pos, vp, viewport, s0))
            return;

        if (Project(px, vp, viewport, sx))
        {
            dl.AddLine(s0, sx, _COL32(255, 0, 0, 255), 3.0f);
            //dl.AddCircleFilled(sx, 6.0f, _COL32(255, 0, 0, 255));
        }

        if (Project(py, vp, viewport, sy))
        {
            dl.AddLine(s0, sy, _COL32(0, 255, 0, 255), 3.0f);
            //dl.AddCircleFilled(sy, 6.0f, _COL32(0, 255, 0, 255));
        }

        if (Project(pz, vp, viewport, sz))
        {
            dl.AddLine(s0, sz, _COL32(0, 128, 255, 255), 3.0f);
            //dl.AddCircleFilled(sz, 6.0f, _COL32(0, 128, 255, 255));
        }
    }

    inline void Draw(GizmoContext& ctx, DebugDrawAPI* dd)
    {
        DrawAxis(dd, ctx);
    }
}