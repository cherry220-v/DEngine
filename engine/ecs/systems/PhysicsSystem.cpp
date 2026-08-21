#include "PhysicsSystem.hpp"
#include <core/EngineContext.hpp>
#include <core/Engine.hpp>

#include <physx/cooking/PxCooking.h>
#include <ecs/components/ColliderComponent.hpp>
#include <ecs/components/CharacterControllerComponent.hpp>
#include <ecs/components/MeshColliderComponent.hpp>
#include <ecs/components/RigidBodyComponent.hpp>

void PhysicsSystem::init(EngineContext* ctx)
{
	context = PhysicsContext();
	initPhysics(context);
	if (ctx->engine->getScene() != nullptr)
	{
		context.scene = createScene(context);
	}
}

void PhysicsSystem::update(Scene& scene, float dt)
{
    auto& physCtx = getContext();

    for (auto e : scene.view<ModelRendererComponent, Transform>())
    {
        auto* renderer = scene.getComponent<ModelRendererComponent>(e);
        if (!renderer || renderer->model.uuid() == "")
            continue;

        auto* meshCollider = scene.getComponent<MeshColliderComponent>(e);
        if (!meshCollider || meshCollider->initialized)
            continue;

        auto* transform = scene.getComponent<Transform>(e);
        glm::mat4 worldMat = scene.getWorldMatrix(e);

        glm::vec3 pos, scale, skew;
        glm::quat rot;
        glm::vec4 perspective;
        glm::decompose(worldMat, scale, rot, pos, skew, perspective);

        physx::PxTransform pxTransform(
            physx::PxVec3(pos.x, pos.y, pos.z),
            physx::PxQuat(rot.x, rot.y, rot.z, rot.w)
        );

        physx::PxMeshScale physxScale(
            physx::PxVec3(scale.x, scale.y, scale.z),
            physx::PxQuat(physx::PxIdentity)
        );

        bool hasRigidBody = scene.hasComponent<RigidBodyComponent>(e);
        physx::PxRigidActor* actor = nullptr;

        if (hasRigidBody)
        {
            auto rigidBody = scene.getComponent<RigidBodyComponent>(e);
            if (!rigidBody->actor)
            {
                rigidBody->actor = physCtx.physics->createRigidDynamic(pxTransform);
            }
            actor = rigidBody->actor;
        }
        else
        {
            actor = physCtx.physics->createRigidStatic(pxTransform);
        }

        if (!actor) continue;

        bool cookingFailed = false;

        if (meshCollider->isConvex)
        {
            if (meshCollider->convexMeshes.empty())
            {
                for (auto mesh : renderer->model.get()->meshes)
                {
                    physx::PxConvexMeshDesc desc;
                    desc.points.count = mesh->getVertexCount();
                    desc.points.stride = sizeof(Vertex);
                    desc.points.data = mesh->getVertices().data();
                    desc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

                    desc.vertexLimit = 255;

                    physx::PxDefaultMemoryOutputStream out;
                    if (!PxCookConvexMesh(*physCtx.cookingParams, desc, out))
                    {
                        cookingFailed = true;
                        break;
                    }

                    physx::PxDefaultMemoryInputData input(out.getData(), out.getSize());
                    physx::PxConvexMesh* convexMesh = physCtx.physics->createConvexMesh(input);

                    if (!convexMesh)
                    {
                        cookingFailed = true;
                        break;
                    }

                    physx::PxConvexMeshGeometry geometry(convexMesh, physxScale);
                    meshCollider->convexMeshes.push_back(convexMesh);

                    auto* shape = physx::PxRigidActorExt::createExclusiveShape(*actor, geometry, *physCtx.defaultMaterial);
                    meshCollider->shapes.push_back(shape);
                }
            }
        }
        else
        {
            if (hasRigidBody)
            {
                std::cerr << "[Physics] ERROR: TriangleMesh cannot be used with Dynamic RigidBodies! Switching to Static." << std::endl;
            }

            if (meshCollider->triangleMeshes.empty())
            {
                for (auto mesh : renderer->model.get()->meshes)
                {
                    physx::PxTriangleMeshDesc desc;
                    desc.points.count = mesh->getVertexCount();
                    desc.points.stride = sizeof(Vertex);
                    desc.points.data = mesh->getVertices().data();

                    desc.triangles.count = mesh->getIndexCount() / 3;
                    desc.triangles.stride = sizeof(uint32_t) * 3;
                    desc.triangles.data = mesh->getIndices().data();

                    physx::PxDefaultMemoryOutputStream out;

                    if (!PxCookTriangleMesh(*physCtx.cookingParams, desc, out))
                    {
                        cookingFailed = true;
                        break;
                    }

                    physx::PxDefaultMemoryInputData input(out.getData(), out.getSize());
                    physx::PxTriangleMesh* triangleMesh = physCtx.physics->createTriangleMesh(input);

                    if (!triangleMesh)
                    {
                        cookingFailed = true;
                        break;
                    }

                    physx::PxTriangleMeshGeometry geometry(triangleMesh, physxScale);
                    meshCollider->triangleMeshes.push_back(triangleMesh);

                    auto* shape = physx::PxRigidActorExt::createExclusiveShape(*actor, geometry, *physCtx.defaultMaterial);

                    shape->setContactOffset(0.01f);
                    meshCollider->shapes.push_back(shape);
                }
            }
        }

        if (cookingFailed)
        {
            actor->release();
            if (hasRigidBody) scene.getComponent<RigidBodyComponent>(e)->actor = nullptr;
            continue;
        }

        physCtx.scene->addActor(*actor);
        meshCollider->actor = actor;
        meshCollider->initialized = true;
        meshCollider->model = renderer->model;
    }

    if (context.scene)
    {
        for (auto e :
            scene.view<
            Transform,
            ColliderComponent,
            CharacterControllerComponent>())
        {
            createCharacterController(scene, e);
        }
        context.scene->simulate(dt);
        context.scene->fetchResults(true);

        for (auto e :
            scene.view<
            Transform,
            CharacterControllerComponent>())
        {
            auto* transform =
                scene.getComponent<Transform>(e);

            auto* controller =
                scene.getComponent<
                CharacterControllerComponent>(e);

            if (!controller->controller)
                continue;

            auto p =
                controller->controller
                ->getPosition();

            transform->position =
            {
                (float)p.x,
                (float)p.y,
                (float)p.z
            };

        }
    }
}

PhysicsSystem::~PhysicsSystem()
{
    shutdownPhysics(context);
}

physx::PxTriangleMesh* PhysicsSystem::cookTriangleMesh(
    const physx::PxTriangleMeshDesc& meshDesc)
{
    if (!meshDesc.isValid())
    {
        std::cout << "MeshDesc invalid\n";
        return nullptr;
    }

    physx::PxDefaultMemoryOutputStream out;

    if (!PxCookTriangleMesh(
        *context.cookingParams,
        meshDesc,
        out))
    {
        std::cout << "Cook failed\n";
        return nullptr;
    }

    physx::PxDefaultMemoryInputData in(
        out.getData(),
        out.getSize());

    auto* mesh =
        context.physics->createTriangleMesh(in);

    std::cout
        << "Cook success, triangles = "
        << meshDesc.triangles.count
        << std::endl;

    return mesh;
}

void PhysicsSystem::createCharacterController(
    Scene& scene,
    Entity e)
{
    auto* transform =
        scene.getComponent<Transform>(e);

    auto* collider =
        scene.getComponent<ColliderComponent>(e);

    auto* character =
        scene.getComponent<CharacterControllerComponent>(e);

    if (!transform || !collider || !character)
        return;

    if (character->controller)
        return;

    physx::PxCapsuleControllerDesc desc;

    desc.material = context.defaultMaterial;

    desc.position =
        physx::PxExtendedVec3(
            transform->position.x,
            transform->position.y,
            transform->position.z
        );

    desc.radius =
        collider->size.x * 0.5f;

    desc.height =
        collider->size.y;

    desc.stepOffset = 0.3f;

    desc.contactOffset = 0.02f;

    desc.upDirection =
        physx::PxVec3(0, 1, 0);

    if (!desc.isValid())
    {
        std::cout
            << "Invalid controller descriptor"
            << std::endl;
        return;
    }

    character->controller =
        context.controllerManager
        ->createController(desc);
}

void PhysicsSystem::moveCharacter(
    Scene& scene,
    Entity e,
    const glm::vec3& movement,
    float dt)
{
    auto* character =
        scene.getComponent<CharacterControllerComponent>(e);

    if (!character || !character->controller)
        return;

    physx::PxControllerFilters filters;

    glm::vec3 displacement = movement;

    if (!character->grounded)
        displacement.y -= character->gravity * dt;

    physx::PxControllerCollisionFlags flags =
        character->controller->move(
            physx::PxVec3(
                displacement.x,
                displacement.y,
                displacement.z
            ),
            0.001f,
            dt,
            filters
        );

    character->grounded =
        flags &
        physx::PxControllerCollisionFlag::eCOLLISION_DOWN;
}