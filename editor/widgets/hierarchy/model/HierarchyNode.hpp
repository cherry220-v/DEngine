#pragma once

#include <ecs/entity/Entity.hpp>
#include <QString>
#include <QIcon>
#include <vector>

struct HierarchyNode
{
    Entity entity;

    QString name;
	QIcon icon;

    Entity parent;
    std::vector<Entity> children;

    bool visible = true;
    bool locked = false;
};