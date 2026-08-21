#pragma once

#include "entity/Entity.hpp"

#include <unordered_map>
#include <vector>
#include <algorithm>

class SceneGraph
{
public:
    struct Node
    {
        Entity entity = NullEntity;
        Entity parent = NullEntity;

        std::vector<Entity> children;
    };

public:
    void addEntity(
        Entity entity,
        Entity parent = NullEntity)
    {
        if (nodes.contains(entity))
            return;

        Node node;
        node.entity = entity;
        node.parent = parent;

        nodes[entity] = node;

        if (parent != NullEntity)
        {
            auto parentIt = nodes.find(parent);

            if (parentIt != nodes.end())
            {
                parentIt->second.children.push_back(entity);
            }
        }
        else
        {
            roots.push_back(entity);
        }
    }

    void removeEntity(Entity entity)
    {
        auto it = nodes.find(entity);

        if (it == nodes.end())
            return;

        auto children = it->second.children;

        for (Entity child : children)
        {
            removeEntity(child);
        }

        Entity parent = it->second.parent;

        if (parent != NullEntity)
        {
            auto& siblings =
                nodes[parent].children;

            siblings.erase(
                std::remove(
                    siblings.begin(),
                    siblings.end(),
                    entity),
                siblings.end());
        }
        else
        {
            roots.erase(
                std::remove(
                    roots.begin(),
                    roots.end(),
                    entity),
                roots.end());
        }

        nodes.erase(it);
    }

    bool hasChild(Entity entity)
    {
        auto it = nodes.find(entity);
        for (auto e : it->second.children)
        {
            if (e == entity)
                return true;
        }
        return false;
    }

    Entity getParent(Entity entity) const
    {
        auto it = nodes.find(entity);

        if (it == nodes.end())
            return NullEntity;

        return it->second.parent;
    }

    const std::vector<Entity>& getChildren(Entity entity) const
    {
        static std::vector<Entity> empty;

        auto it = nodes.find(entity);

        if (it == nodes.end())
            return empty;

        return it->second.children;
    }

    const std::vector<Entity>& getRoots() const
    {
        return roots;
    }

    bool contains(Entity entity) const
    {
        return nodes.contains(entity);
    }

    void setParent(
        Entity entity,
        Entity parent)
    {
        if (entity == parent)
            return;

        auto it = nodes.find(entity);

        if (it == nodes.end())
            return;

        Entity oldParent =
            it->second.parent;

        if (oldParent == parent)
            return;

        if (oldParent != NullEntity)
        {
            auto& children =
                nodes[oldParent].children;

            children.erase(
                std::remove(
                    children.begin(),
                    children.end(),
                    entity),
                children.end());
        }
        else
        {
            roots.erase(
                std::remove(
                    roots.begin(),
                    roots.end(),
                    entity),
                roots.end());
        }

        it->second.parent = parent;

        if (parent != NullEntity)
        {
            nodes[parent].children.push_back(entity);
        }
        else
        {
            roots.push_back(entity);
        }
    }

private:
    std::unordered_map<Entity, Node> nodes;

    std::vector<Entity> roots;
};