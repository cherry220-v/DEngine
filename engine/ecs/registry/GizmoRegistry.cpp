#include "GizmoRegistry.hpp"


void GizmoRegistry::registerGizmo(const std::string& componentName, std::unique_ptr<IEditorGizmo> gizmo)
{
	m_gizmos[componentName] = std::move(gizmo);
}

IEditorGizmo* GizmoRegistry::getGizmo(const std::string& componentName)
{
	auto it = m_gizmos.find(componentName);
	if (it == m_gizmos.end()) return nullptr;
	return it->second.get();
}

const std::unordered_map<std::string, std::unique_ptr<IEditorGizmo>>& GizmoRegistry::gizmos() const
{
	return m_gizmos;
}
