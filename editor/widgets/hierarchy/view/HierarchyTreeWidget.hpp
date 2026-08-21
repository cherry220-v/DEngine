#pragma once

#include <QTreeView>
#include <QWidget>

#include "../model/HierarchyNode.hpp"
#include "../model/SceneHierarchyModel.hpp"

#include "../commands/SceneCommands.hpp"

#include <ecs/entity/Entity.hpp>

class Scene;
class EditorContext;

class HierarchyTreeWidget : public QTreeView
{
    Q_OBJECT

public:
    explicit HierarchyTreeWidget(QWidget* parent = nullptr);

    void setScene(Scene* scene);
    void setContext(EditorContext* ctx);

    void setModel(SceneHierarchyModel* model);

signals:
    void entitySelected(Entity e);
    void entityDeleted(Entity e);
    void entityToggledVisibility(Entity e, bool visible);
    void entityToggledLock(Entity e, bool locked);

private slots:
    void onCellClicked(const QModelIndex& index);
    void showContextMenu(const QPoint& pos);

private:
    Scene* m_scene = nullptr;
    EditorContext* m_ctx = nullptr;
    SceneHierarchyModel* m_model = nullptr;

    SceneCommands* m_commands = nullptr;

	QWidget* parent;
};