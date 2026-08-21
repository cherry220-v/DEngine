#include "ObjHierarchyView.h"

#include <ecs/components/IDComponent.hpp>
#include <ecs/components/NameComponent.hpp>
#include <ecs/Scene.hpp>
#include <core/EngineContext.hpp>
#include "../engine/EditorApp.hpp"

ObjHierarchyView::ObjHierarchyView(QWidget* parent) : parent(parent)
{
    header()->hide();
}

QTreeWidgetItem* ObjHierarchyView::add(const ObjData& data, QTreeWidgetItem* parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent);

    HierarchyObject* customWidget = new HierarchyObject(data.text);

    this->addTopLevelItem(item);
    this->setItemWidget(item, 0, customWidget);

    connect(customWidget, &HierarchyObject::requestDelete, [this, item]() {
        delete item;
        });
    return item;
}

void buildNode(
    Entity e,
    QTreeWidgetItem* parent
)
{
    Application& app = EditorApp::instance();
	Scene* scene = app.engine()->getScene();
    auto* name =
        scene->getComponent<NameComponent>(e);

    auto* id =
        scene->getComponent<IDComponent>(e);

    auto* item =
        new QTreeWidgetItem();

    item->setText(
        0,
        QString::fromStdString(
            name ? name->name : "Entity"
        )
    );

    item->setData(
        0,
        Qt::UserRole,
        QString::fromStdString(id->id)
    );

    parent->addChild(item);

    for (Entity child : scene->getSceneGraph().getChildren(e))
    {
        buildNode(child, item);
    }
}