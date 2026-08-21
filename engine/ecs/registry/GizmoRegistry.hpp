#pragma once

#include <editor/gizmos/IEditorGizmo.hpp>

#define REGISTER_GIZMO(name, gizmoPtr)           \
    static bool _gizmo_reg_##__LINE__ = []()     \
    {                                             \
        GizmoRegistry::get().registerGizmo(       \
            name,                                 \
            std::move(gizmoPtr)                   \
        );                                        \
        return true;                              \
    }()

class GizmoRegistry
{
public:
    static GizmoRegistry& get() { static GizmoRegistry s; return s; }

public:
    void registerGizmo(
        const std::string& componentName,
        std::unique_ptr<IEditorGizmo> gizmo
    );

    IEditorGizmo* getGizmo(
        const std::string& componentName
    );

    const std::unordered_map<std::string, std::unique_ptr<IEditorGizmo>>& gizmos() const;

private:
    GizmoRegistry() = default;
    ~GizmoRegistry() = default;

    GizmoRegistry(
        const GizmoRegistry&
    ) = delete;

    GizmoRegistry& operator=(
        const GizmoRegistry&
        ) = delete;

private:
    std::unordered_map<
        std::string,
        std::unique_ptr<IEditorGizmo>
    > m_gizmos;
};
