#pragma once

#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include <cstdint>
#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#define DRAW_PI 3.14159265358979323846f
inline glm::vec4 makePoint(float x, float y, float z)
{
    return glm::vec4(x, y, z, 1.0f);
}

inline glm::vec4 makeDir(float x, float y, float z)
{
    return glm::vec4(x, y, z, 0.0f);
}

class Texture;
using TextureRef = std::shared_ptr<Texture>;

typedef unsigned int        WidgetID;// A unique ID used by widgets (typically the result of hashing a stack of string)
typedef signed char         S8;   // 8-bit signed integer
typedef unsigned char       U8;   // 8-bit unsigned integer
typedef signed short        S16;  // 16-bit signed integer
typedef unsigned short      U16;  // 16-bit unsigned integer
typedef signed int          S32;  // 32-bit signed integer == int
typedef unsigned int        U32;  // 32-bit unsigned integer (often used to store packed colors)
typedef signed   long long  S64;  // 64-bit signed integer
typedef unsigned long long  U64;  // 64-bit unsigned integer
struct Float4 { float r, g, b, a; };

typedef int DrawFlags;

struct DrawVert
{
    float pos[2];
    float uv[2];
    uint32_t col;

    float depth;
};

struct Rect
{
    glm::vec2 Min;
    glm::vec2 Max;
    Rect() = default;
    Rect(const glm::vec2& min, const glm::vec2& max) : Min(min), Max(max) {}

    bool Contains(const glm::vec2& p) const
    {
        return p.x >= Min.x && p.y >= Min.y && p.x < Max.x && p.y < Max.y;
    }
};

struct ScreenRect3D
{
    glm::vec2 min;
    glm::vec2 max;
    bool valid = false;
};

struct Ray
{
    glm::vec3 origin;
    glm::vec3 direction;

    glm::vec3 invDirection;

    void finalize()
    {
        invDirection = 1.0f / direction;
    }

    glm::vec3 at(float t) const
    {
        return origin + direction * t;
    }
};

using DrawIdx = uint32_t;

enum class DrawSpace
{
    Screen,
    World,
    WorldOverlay
};

enum class Space
{
    Local,
    World,
    View,
    Clip,
    Screen
};

struct DrawCmd
{
    uint32_t elemCount;

    uint32_t idxOffset;
    uint32_t vtxOffset;

    uint32_t textureId;

    Rect clipRect;

    DrawSpace space;

    float sortKey;

    bool depthTest = false;
    bool writeDepth = false;
    bool pickable = false;
};

enum DrawFlags_
{
    DrawFlags_None = 0,
    DrawFlags_Closed = 1 << 0, // Замкнуть контур (соединить последнюю точку с первой)
    DrawFlags_RoundCornersTopLeft = 1 << 4, // Округлить только верхний левый угол
    DrawFlags_RoundCornersTopRight = 1 << 5, // Округлить только верхний правый угол
    DrawFlags_RoundCornersBottomLeft = 1 << 6, // Округлить только нижний левый угол
    DrawFlags_RoundCornersBottomRight = 1 << 7, // Округлить только нижний правый угол

    DrawFlags_RoundCornersTop = DrawFlags_RoundCornersTopLeft | DrawFlags_RoundCornersTopRight,
    DrawFlags_RoundCornersBottom = DrawFlags_RoundCornersBottomLeft | DrawFlags_RoundCornersBottomRight,
    DrawFlags_RoundCornersLeft = DrawFlags_RoundCornersTopLeft | DrawFlags_RoundCornersBottomLeft,
    DrawFlags_RoundCornersRight = DrawFlags_RoundCornersTopRight | DrawFlags_RoundCornersBottomRight,
    DrawFlags_RoundCornersAll = DrawFlags_RoundCornersTopLeft | DrawFlags_RoundCornersTopRight | DrawFlags_RoundCornersBottomLeft | DrawFlags_RoundCornersBottomRight,
};

#define _COL32_R_SHIFT    0
#define _COL32_G_SHIFT    8
#define _COL32_B_SHIFT    16
#define _COL32_A_SHIFT    24

#define _COL32(R,G,B,A)    (((U32)(A)<<_COL32_A_SHIFT) | ((U32)(B)<<_COL32_B_SHIFT) | ((U32)(G)<<_COL32_G_SHIFT) | ((U32)(R)<<_COL32_R_SHIFT))
static U32 _COL32_WHITE = _COL32(255, 255, 255, 255);
static U32 _COL32_BLACK = _COL32(0, 0, 0, 255);

struct DrawContext
{
    glm::mat4 view;
    glm::mat4 proj;

    glm::mat4 viewProj;
    glm::mat4 invView;
    glm::mat4 invProj;
    glm::mat4 invViewProj;

    glm::vec2 viewportSize;
};

struct OverlayRenderState
{
    DrawContext ctx;

    enum Mode {
        UI,
        WORLD,
        WORLD_OVERLAY
    } mode;
};