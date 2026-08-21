#pragma once

#include "HierarchyObject.h"
#include <QTreeWidget>
#include <QTreeView>
#include <QHeaderView>
#include <QList>
#include <QString>

#include <ecs/entity/Entity.hpp>

class ObjData {
public:
	QString text;
};

class ObjHierarchyView : public QTreeWidget
{
	Q_OBJECT

public:
	explicit ObjHierarchyView(QWidget* parent = nullptr);
	~ObjHierarchyView() = default;

	QTreeWidgetItem* add(const ObjData& data, QTreeWidgetItem* parent = nullptr);

protected:

private:
	QList<HierarchyObject*> hierarchuObjects;

	QWidget* parent;
};

void buildNode(
	Entity e,
	QTreeWidgetItem* parent
);