#include <physx/PxPhysicsAPI.h>
#include <physx/cooking/PxCooking.h>
#include <physx/characterkinematic/PxControllerManager.h>

using namespace physx;

struct PhysicsContext
{
    PxDefaultAllocator      allocator;
    PxDefaultErrorCallback  errorCallback;

    PxFoundation* foundation = nullptr;
    PxPhysics* physics = nullptr;
    PxScene* scene = nullptr;
    PxCpuDispatcher* dispatcher = nullptr;
    PxPvd* pvd = nullptr;
    PxControllerManager* controllerManager = nullptr;

    PxCookingParams* cookingParams = nullptr;

    PxMaterial* defaultMaterial = nullptr;
};

static void initPhysics(PhysicsContext& ctx)
{
    if (ctx.foundation != nullptr) return;
    ctx.foundation =
        PxCreateFoundation(
            PX_PHYSICS_VERSION,
            ctx.allocator,
            ctx.errorCallback);

    ctx.physics =
        PxCreatePhysics(
            PX_PHYSICS_VERSION,
            *ctx.foundation,
            PxTolerancesScale(),
            true);

    ctx.cookingParams = new PxCookingParams(ctx.physics->getTolerancesScale());
    ctx.defaultMaterial = ctx.physics->createMaterial(0.0f, 0.0f, 0.0f);
    ctx.defaultMaterial->setFrictionCombineMode(physx::PxCombineMode::eMIN);
}

static PxScene* createScene(PhysicsContext& ctx)
{
    PxSceneDesc sceneDesc(ctx.physics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.flags |= physx::PxSceneFlag::eENABLE_PCM;

    ctx.dispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = ctx.dispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;

    ctx.scene = ctx.physics->createScene(sceneDesc);
    ctx.controllerManager =
        PxCreateControllerManager(*ctx.scene);
    return ctx.scene;
}

static void shutdownPhysics(PhysicsContext& ctx)
{
    if (ctx.scene) { ctx.scene->release();      ctx.scene = nullptr; }
    if (ctx.dispatcher) { delete ctx.dispatcher; ctx.dispatcher = nullptr; }

    if (ctx.cookingParams) { delete ctx.cookingParams; ctx.cookingParams = nullptr; }

    if (ctx.physics) { ctx.physics->release();    ctx.physics = nullptr; }
    if (ctx.foundation) { ctx.foundation->release(); ctx.foundation = nullptr; }
}