#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <limits>

namespace OverlayMath
{
    struct Ray
    {
        glm::vec3 origin;
        glm::vec3 dir; // normalized
    };

    struct Plane
    {
        glm::vec3 point;
        glm::vec3 normal;
    };

    struct ScreenParams
    {
        glm::vec2 viewport; // width, height
        glm::mat4 view;
        glm::mat4 proj;
    };

    // -------------------------
    // 1. World <-> Screen
    // -------------------------

    inline glm::vec3 WorldToNDC(const glm::vec3& p, const glm::mat4& view, const glm::mat4& proj)
    {
        glm::vec4 clip = proj * view * glm::vec4(p, 1.0f);
        if (clip.w == 0.0f) return glm::vec3(0.0f);

        return glm::vec3(clip) / clip.w;
    }

    inline glm::vec2 WorldToScreen(const glm::vec3& p, const ScreenParams& sp)
    {
        glm::vec3 ndc = WorldToNDC(p, sp.view, sp.proj);

        return {
            (ndc.x * 0.5f + 0.5f) * sp.viewport.x,
            (1.0f - (ndc.y * 0.5f + 0.5f)) * sp.viewport.y
        };
    }

    inline glm::vec3 ScreenToWorldNear(const glm::vec2& screen, float depth01, const ScreenParams& sp)
    {
        glm::vec4 ndc;
        ndc.x = (screen.x / sp.viewport.x) * 2.0f - 1.0f;
        ndc.y = 1.0f - (screen.y / sp.viewport.y) * 2.0f - 1.0f;
        ndc.z = depth01 * 2.0f - 1.0f;
        ndc.w = 1.0f;

        glm::mat4 invVP = glm::inverse(sp.proj * sp.view);
        glm::vec4 world = invVP * ndc;

        return world / world.w;
    }

    // -------------------------
    // 2. Ray casting
    // -------------------------

    inline Ray ScreenPointToRay(const glm::vec2& screen, const ScreenParams& sp)
    {
        glm::vec3 nearP = ScreenToWorldNear(screen, 0.0f, sp);
        glm::vec3 farP = ScreenToWorldNear(screen, 1.0f, sp);

        Ray r;
        r.origin = nearP;
        r.dir = glm::normalize(farP - nearP);
        return r;
    }

    inline bool RayPlaneIntersect(const Ray& ray, const Plane& plane, float& t)
    {
        float denom = glm::dot(plane.normal, ray.dir);
        if (glm::abs(denom) < 1e-6f) return false;

        t = glm::dot(plane.point - ray.origin, plane.normal) / denom;
        return t >= 0.0f;
    }

    inline glm::vec3 ClosestPointOnRay(const Ray& ray, const glm::vec3& point)
    {
        float t = glm::dot(point - ray.origin, ray.dir);
        return ray.origin + ray.dir * t;
    }

    inline glm::vec3 ClosestPointOnLine(const glm::vec3& a, const glm::vec3& b, const glm::vec3& p)
    {
        glm::vec3 ab = b - a;
        float t = glm::dot(p - a, ab) / glm::dot(ab, ab);
        return a + glm::clamp(t, 0.0f, 1.0f) * ab;
    }

    // -------------------------
    // 3. Distance (picking)
    // -------------------------

    inline float DistancePointToLine2D(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b)
    {
        glm::vec2 ab = b - a;
        glm::vec2 ap = p - a;

        float t = glm::dot(ap, ab) / glm::dot(ab, ab);
        glm::vec2 c = a + glm::clamp(t, 0.0f, 1.0f) * ab;

        return glm::length(p - c);
    }

    inline float DistancePointToRay3D(const glm::vec3& p, const Ray& r)
    {
        glm::vec3 ap = p - r.origin;
        float t = glm::dot(ap, r.dir);
        glm::vec3 closest = r.origin + r.dir * t;
        return glm::length(p - closest);
    }

    // -------------------------
    // 4. Axis constraints (core gizmo logic)
    // -------------------------

    inline glm::vec3 ProjectOnAxis(const glm::vec3& point, const glm::vec3& origin, const glm::vec3& axis)
    {
        glm::vec3 ap = point - origin;
        float t = glm::dot(ap, axis);
        return origin + axis * t;
    }

    inline glm::vec3 ProjectRayOnAxis(const Ray& ray, const glm::vec3& origin, const glm::vec3& axis)
    {
        // Closest point between ray and axis line approximation
        glm::vec3 p = ClosestPointOnRay(ray, origin);
        return ProjectOnAxis(p, origin, axis);
    }

    // -------------------------
    // 5. Plane constraints (translate gizmo planes)
    // -------------------------

    inline glm::vec3 RayPlanePoint(const Ray& ray, const Plane& plane)
    {
        float t;
        if (!RayPlaneIntersect(ray, plane, t))
            return ray.origin;

        return ray.origin + ray.dir * t;
    }

    // -------------------------
    // 6. Rotation helpers
    // -------------------------

    inline float SignedAngle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& normal)
    {
        float angle = glm::atan(glm::length(glm::cross(a, b)), glm::dot(a, b));
        float sign = glm::sign(glm::dot(normal, glm::cross(a, b)));
        return angle * sign;
    }

    inline glm::vec3 ProjectOnPlane(const glm::vec3& v, const glm::vec3& normal)
    {
        return v - normal * glm::dot(v, normal);
    }

    // -------------------------
    // 7. Camera helpers
    // -------------------------

    inline glm::vec3 CameraRight(const glm::mat4& view)
    {
        return glm::vec3(view[0][0], view[1][0], view[2][0]);
    }

    inline glm::vec3 CameraUp(const glm::mat4& view)
    {
        return glm::vec3(view[0][1], view[1][1], view[2][1]);
    }

    inline glm::vec3 CameraForward(const glm::mat4& view)
    {
        return -glm::vec3(view[0][2], view[1][2], view[2][2]);
    }

    // -------------------------
    // 8. Gizmo screen-space scaling (important for Unity-like feel)
    // -------------------------

    inline float GetScreenScale(const glm::vec3& worldPos, const ScreenParams& sp, float baseScale = 1.0f)
    {
        glm::vec3 ndc = WorldToNDC(worldPos, sp.view, sp.proj);
        float depth = glm::clamp(ndc.z, 0.0f, 1.0f);

        // simple perspective compensation
        return baseScale * (1.0f + depth * 5.0f);
    }
    inline glm::vec2 WorldToScreen(
        const glm::vec3& worldPos,
        const glm::mat4& viewProj,
        const glm::vec2& viewport)
    {
        glm::vec4 clip = viewProj * glm::vec4(worldPos, 1.0f);

        if (clip.w == 0.0f)
            return glm::vec2(-1, -1);

        glm::vec3 ndc = glm::vec3(clip) / clip.w;

        return {
            (ndc.x * 0.5f + 0.5f) * viewport.x,
            (1.0f - (ndc.y * 0.5f + 0.5f)) * viewport.y
        };
    }

    inline glm::vec3 ScreenRayDir(
        glm::vec2 mouse,
        glm::vec2 viewport,
        const glm::mat4& invViewProj)
    {
        glm::vec2 ndc(
            (2.0f * mouse.x) / viewport.x - 1.0f,
            1.0f - (2.0f * mouse.y) / viewport.y
        );

        glm::vec4 near = invViewProj * glm::vec4(ndc.x, ndc.y, -1, 1);
        glm::vec4 far = invViewProj * glm::vec4(ndc.x, ndc.y, 1, 1);

        near /= near.w;
        far /= far.w;

        return glm::normalize(glm::vec3(far - near));
    }

    inline glm::vec3 ScreenRayOrigin(
        glm::vec2 mouse,
        glm::vec2 viewport,
        const glm::mat4& invViewProj)
    {
        glm::vec2 ndc(
            (2.0f * mouse.x) / viewport.x - 1.0f,
            1.0f - (2.0f * mouse.y) / viewport.y
        );

        glm::vec4 origin = invViewProj * glm::vec4(ndc.x, ndc.y, -1, 1);
        return glm::vec3(origin / origin.w);
    }
    inline float ComputeGizmoScale(
        const glm::vec3& pos,
        const glm::mat4& viewProj,
        const glm::vec2& viewport,
        float pixels = 100.0f)
    {
        glm::vec3 camPos = glm::inverse(viewProj)[3]; // грубо (лучше взять camera pos)

        float dist = glm::length(camPos - pos);
        return dist * 0.2f; // стабильный world-scale
    }
}