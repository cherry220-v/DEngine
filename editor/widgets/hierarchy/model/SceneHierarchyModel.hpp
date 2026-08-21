#pragma once
#include <QAbstractItemModel>
#include <ecs/entity/Entity.hpp>
#include <unordered_map>
#include <vector>

#include "HierarchyNode.hpp"

class Scene;

class SceneHierarchyModel : public QAbstractItemModel
{
public:
    void buildFromScene(Scene* scene);

	void addNode(const HierarchyNode& node);

	void resolveTree();
	void clear();

    const HierarchyNode* get(Entity e) const;

    const std::vector<Entity>& getRoots() const;
    const std::vector<Entity>& getChildren(Entity e) const;
public:

    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;

    QVariant data(const QModelIndex& index, int role) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

private:
    std::unordered_map<Entity, HierarchyNode> nodes;
    std::vector<Entity> rootEntities;
};