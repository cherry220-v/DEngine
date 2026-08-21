#include "EditorDrawList.hpp"

#include <render/renderer/IRenderDevice.hpp>

#include <math/Vertex.h>
#include <math/CameraBasis.hpp>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <cmath>

#include <iostream>

glm::vec3 computeLineOffset(
    const glm::vec3& a,
    const glm::vec3& b,
    const glm::mat4& view,
    const glm::mat4& proj,
    float viewportWidth,
    float viewportHeight,
    float pixelThickness)
{
    glm::vec4 clipA = proj * view * glm::vec4(a, 1.0f);
    glm::vec4 clipB = proj * view * glm::vec4(b, 1.0f);

    glm::vec2 ndcA = glm::vec2(clipA) / clipA.w;
    glm::vec2 ndcB = glm::vec2(clipB) / clipB.w;

    glm::vec2 screenA = (ndcA * 0.5f + 0.5f) *
        glm::vec2(viewportWidth, viewportHeight);

    glm::vec2 screenB = (ndcB * 0.5f + 0.5f) *
        glm::vec2(viewportWidth, viewportHeight);

    glm::vec2 dir = glm::normalize(screenB - screenA);
    glm::vec2 right = glm::vec2(-dir.y, dir.x);

    float depthScale = (clipA.w + clipB.w) * 0.5f;

    float scale = (pixelThickness / viewportHeight) * depthScale;

    glm::vec3 camRight =
        glm::normalize(glm::vec3(glm::inverse(view)[0]));

    return camRight * scale;
}

static void drawLineImpl(
    std::vector<GizmoVertex>& vertices,
    const glm::vec3& a,
    const glm::vec3& b,
    const glm::vec4& c)
{
    vertices.push_back({
        a,
        c
        });

    vertices.push_back({
        b,
        c
        });
}

static void drawCircleImpl(
    std::vector<GizmoVertex>& vertices,
    const glm::vec3& center,
    const glm::vec3& normal,
    float radius,
    const glm::vec4& color)
{
    const int segments = 32;

    glm::vec3 n = glm::normalize(normal);

    glm::vec3 up = (std::abs(n.y) < 0.99f) ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
    glm::vec3 right = glm::normalize(glm::cross(up, n));
    glm::vec3 forward = glm::cross(n, right);

    float step = glm::two_pi<float>() / segments;

    glm::vec3 prev =
        center + right * radius;

    for (int i = 1; i <= segments; i++)
    {
        float a = i * step;

        glm::vec3 p =
            center +
            (std::cos(a) * right + std::sin(a) * forward) * radius;

        drawLineImpl(vertices, prev, p, color);
        prev = p;
    }
}

static void addCone3D(
    std::vector<GizmoVertex>& v,
    const glm::vec3& pos,
    const glm::vec3& dir,
    float radius,
    float height,
    const glm::vec4& color,
    const CameraBasis& cam)
{
    const int segments = 24;

    glm::vec3 d = glm::normalize(dir);

    glm::vec3 right = cam.right;
    glm::vec3 up = cam.up;

    glm::vec3 tip = pos + d * height;

    glm::vec3 prev =
        pos + right * radius;

    for (int i = 1; i <= segments; i++)
    {
        float a = (float)i / segments * glm::two_pi<float>();

        glm::vec3 offset =
            (std::cos(a) * right + std::sin(a) * up) * radius;

        glm::vec3 p = pos + offset;

        // side triangles
        v.push_back({ pos, color });
        v.push_back({ prev, color });
        v.push_back({ p, color });

        // cap
        v.push_back({ tip, color });
        v.push_back({ p, color });
        v.push_back({ prev, color });

        prev = p;
    }
}

static void addThickLine3D(
    std::vector<GizmoVertex>& v,
    const glm::vec3& a,
    const glm::vec3& b,
    const CameraBasis& cam,
    float thickness,
    const glm::vec4& color)
{
    glm::vec3 dir = glm::normalize(b - a);

    glm::vec3 right = glm::normalize(glm::cross(dir, cam.forward));
    glm::vec3 offset = right * thickness;

    glm::vec3 a1 = a + offset;
    glm::vec3 a2 = a - offset;
    glm::vec3 b1 = b + offset;
    glm::vec3 b2 = b - offset;

    v.push_back({ a1, color });
    v.push_back({ b1, color });
    v.push_back({ a2, color });

    v.push_back({ a2, color });
    v.push_back({ b1, color });
    v.push_back({ b2, color });
}

static void addArrow(
    std::vector<GizmoVertex>& v,
    const glm::vec3& a,
    const glm::vec3& b,
    const CameraBasis& cam,
    float thickness,
    const glm::vec4& color)
{
    glm::vec3 dir = glm::normalize(b - a);
    float len = glm::length(b - a);

    float head = len * 0.2f;
    glm::vec3 shaftEnd = b - dir * head;

    addThickLine3D(v, a, shaftEnd, cam, thickness, color);

    glm::vec3 right = cam.right;
    glm::vec3 up = cam.up;

    glm::vec3 base = shaftEnd;
    glm::vec3 tip = b;

    float w = head * 0.5f;

    glm::vec3 p1 = base + right * w;
    glm::vec3 p2 = base - right * w;
    glm::vec3 p3 = base + up * w;

    v.push_back({ tip, color });
    v.push_back({ p1, color });
    v.push_back({ p2, color });

    v.push_back({ tip, color });
    v.push_back({ p2, color });
    v.push_back({ p3, color });

    v.push_back({ tip, color });
    v.push_back({ p3, color });
    v.push_back({ p1, color });
}

void renderEditorDrawList(
    const EditorDrawList& drawList,
    const glm::mat4& view,
    const glm::mat4& proj,
    float viewportWidth,
    float viewportHeight,
    IRenderDevice* device)
{
    std::vector<GizmoVertex> v;

    CameraBasis cam = getCameraBasis(view);

    glm::vec3 camPos =
        glm::vec3(glm::inverse(view)[3]);

    float scale = getGizmoScale(camPos, view, viewportHeight);

    // LINES
    for (auto& l : drawList.getLines())
    {
        addThickLine3D(
            v,
            l.a,
            l.b,
            cam,
            l.thickness * scale,
            l.color
        );
    }

    // CONES
    for (auto& c : drawList.getCones())
    {
        addCone3D(
            v,
            c.pos,
            c.dir,
            c.radius * scale,
            c.height * scale,
            c.color,
            cam
        );
    }

    // ARROWS
    for (auto& a : drawList.getArrows())
    {
        addArrow(
            v,
            a.a,
            a.b,
            cam,
            a.thickness * scale,
            a.color
        );
    }

    glBufferData(
        GL_ARRAY_BUFFER,
        v.size() * sizeof(GizmoVertex),
        v.data(),
        GL_DYNAMIC_DRAW
    );

    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)v.size());
}
