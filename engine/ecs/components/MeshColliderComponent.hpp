#pragma once

#include "IComponent.hpp"
#include "../registry/ComponentInfo.hpp"
#include "../registry/ComponentRegistry.hpp"

#include <assets/AssetRef.hpp>

#include <models/Model.hpp>

#include <physx/PxShape.h>
#include <physx/PxActor.h>
#include <vector>

struct MeshColliderComponent : public IComponent {

    AssetRef<Model> model;

    physx::PxRigidActor* actor = nullptr;

    bool isConvex = false;

    bool initialized = false;
    bool needsRebuild = false;

    std::vector<physx::PxTriangleMesh*> triangleMeshes;
    std::vector<physx::PxConvexMesh*> convexMeshes;

    std::vector<physx::PxShape*> shapes;

};

static ComponentInfo meshColliderComponentInfo
{
    "MeshColliderComponent",
    {
    }
};

REGISTER_COMPONENT(meshColliderComponentInfo);