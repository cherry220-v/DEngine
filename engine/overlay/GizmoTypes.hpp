#pragma once
#include <glm/glm.hpp>

enum class GizmoMode
{
    None,
    Translate,
    Rotate,
    Scale
};

enum class GizmoAxis
{
    None,
    X,
    Y,
    Z,
    XY,
    YZ,
    XZ
};

enum class GizmoState
{
    Idle,
    Hover,
    Dragging
};