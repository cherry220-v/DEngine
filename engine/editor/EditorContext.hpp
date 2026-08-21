#pragma once

#include <core/platform/input/IInput.hpp>

#include <ecs/entity/Entity.hpp>

#include <editor/EditorDrawList.hpp>

#include "GizmoAxis.hpp"

struct EditorContext
{
    bool editorMode = false;

    EditorDrawList* drawList = nullptr;

    IInput* input = nullptr;

    Entity editorCamera = INVALID_ENTITY;

    Entity selectedEntity = INVALID_ENTITY;

    Ray mouseRay;

    GizmoAxis activeAxis =
        GizmoAxis::None;

    bool dragging =
        false;
};