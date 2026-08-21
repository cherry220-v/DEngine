#include "HierarchyBuilder.hpp"
#include <ecs/Scene.hpp>
#include "../model/SceneHierarchyModel.hpp"
#include "../model/HierarchyNode.hpp"

#include <ecs/components/HierarchyComponent.hpp>
#include <ecs/components/NameComponent.hpp>

void HierarchyBuilder::build(Scene* scene, SceneHierarchyModel& model)
{
    model.clear();

    for (auto e : scene->getEntityManager().aliveEntities())
    {
        auto* h = scene->getComponent<HierarchyComponent>(e);
        auto* n = scene->getComponent<NameComponent>(e);

        HierarchyNode node;

        node.entity = e;

        if (h)
            node.parent = h->parent;

        if (n)
            node.name = QString::fromUtf8(n->name);

        model.addNode(node);
    }

    model.resolveTree();
}