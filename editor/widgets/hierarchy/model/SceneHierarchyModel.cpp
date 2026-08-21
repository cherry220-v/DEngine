#include "SceneHierarchyModel.hpp"
#include "../tools/HierarchyBuilder.hpp"

#include <QIcon>
#include <QtCore/Qt>

void SceneHierarchyModel::buildFromScene(Scene* scene)
{
    beginResetModel();
    HierarchyBuilder::build(scene, *this);
    endResetModel();
}

void SceneHierarchyModel::resolveTree()
{
    rootEntities.clear();

    for (auto& [e, node] : nodes)
        node.children.clear();

    for (auto& [e, node] : nodes)
    {
        if (node.parent != INVALID_ENTITY && nodes.contains(node.parent))
        {
            nodes[node.parent].children.push_back(e);
        }
        else
        {
            rootEntities.push_back(e);
        }
    }
}

void SceneHierarchyModel::addNode(const HierarchyNode& node)
{
    nodes[node.entity] = node;
}

const std::vector<Entity>& SceneHierarchyModel::getChildren(Entity e) const
{
    static std::vector<Entity> empty;

    auto it = nodes.find(e);
    if (it == nodes.end())
        return empty;

    return it->second.children;
}

const std::vector<Entity>& SceneHierarchyModel::getRoots() const
{
    return rootEntities;
}

void SceneHierarchyModel::clear()
{
    nodes.clear();
    rootEntities.clear();
}

const HierarchyNode* SceneHierarchyModel::get(Entity e) const
{
    auto it = nodes.find(e);
    if (it == nodes.end())
        return nullptr;
    return &it->second;
}

int SceneHierarchyModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid() && parent.column() > 0)
        return 0;

    if (!parent.isValid())
    {
        return rootEntities.size();
    }

        auto* ptr = parent.internalPointer();
        auto* node = static_cast<HierarchyNode*>(ptr);
        qDebug() << node;
        if (node != nullptr)
        {
            return node->children.size();
        }
}

int SceneHierarchyModel::columnCount(const QModelIndex& parent) const
{
	return 4; // Name, Icon, Visibility, Lock
}

QModelIndex SceneHierarchyModel::index(int row, int column, const QModelIndex& parent) const
{
    if (row < 0 || column < 0 || column >= columnCount(parent))
        return {};


    if (!parent.isValid())
    {
        Entity e = rootEntities[row];
        return createIndex(row, column, (void*)&nodes.at(e));
    }

    auto* parentNode = static_cast<HierarchyNode*>(parent.internalPointer());
    Entity child = parentNode->children[row];

    return createIndex(row, column, &nodes.at(child));
}

QModelIndex SceneHierarchyModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return {};

    auto* node = static_cast<HierarchyNode*>(index.internalPointer());

    Entity parentEntity = node->parent;

    if (parentEntity == INVALID_ENTITY)
        return {};

    const HierarchyNode& parentNode = nodes.at(parentEntity);

    int row = -1;

    if (parentNode.parent == INVALID_ENTITY)
    {
        auto it = std::find(rootEntities.begin(),
            rootEntities.end(),
            parentEntity);

        if (it == rootEntities.end())
            return {};

        row = std::distance(rootEntities.begin(), it);
    }
    else
    {
        const auto& siblings = nodes.at(parentNode.parent).children;

        auto it = std::find(siblings.begin(),
            siblings.end(),
            parentEntity);

        if (it == siblings.end())
            return {};

        row = std::distance(siblings.begin(), it);
    }

    return createIndex(row, 0, (void*)&nodes.at(parentEntity));
}

QVariant SceneHierarchyModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return {};

    auto* node = static_cast<HierarchyNode*>(index.internalPointer());

    int col = index.column();

    if (role == Qt::TextAlignmentRole)
    {
        if (col == 0)
            return Qt::AlignLeft;
        else
            return Qt::AlignCenter;
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (col == 0)
            return node->name;
        return {};
    }

    if (role == Qt::DecorationRole)
    {
        if (col == 0)
            return QIcon(":/hierarchy/cube.svg");

        if (col == 1)
            if (node->visible)
                return QIcon(":/hierarchy/eye.svg");
            else
                return QIcon(":/hierarchy/eye-closed.svg");

        if (col == 2)
            if (node->locked)
                return QIcon(":/hierarchy/lock.svg");
            else
                return QIcon(":/hierarchy/lock-unlocked.svg");

        if (col == 3)
            return QIcon(":/hierarchy/trash.svg");
    }

    return {};
}

Qt::ItemFlags SceneHierarchyModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (index.column() == 0)
        f |= Qt::ItemIsEditable;

    if (index.column() == 1)
        f |= Qt::ItemIsUserCheckable;

    return f;
}

bool SceneHierarchyModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid())
        return false;

    auto* nodePtr = static_cast<HierarchyNode*>(index.internalPointer());
    if (!nodePtr)
        return false;

    if (role == Qt::CheckStateRole)
    {
        nodePtr->visible = (value.toInt() == Qt::Checked);

        emit dataChanged(index, index, { Qt::CheckStateRole });
        return true;
    }

    if (role == Qt::EditRole && index.column() == 0)
    {
        nodePtr->name = value.toString();

        emit dataChanged(index, index, { Qt::DisplayRole, Qt::EditRole });

        return true;
    }

    return false;
}
