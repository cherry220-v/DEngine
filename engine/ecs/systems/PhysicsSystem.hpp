#pragma once

#include "ISystem.hpp"
#include "../registry/SystemInfo.hpp"
#include "../registry/SystemRegistry.hpp"

#include <ecs/entity/Entity.hpp>
#include <glm/vec3.hpp>

#include <physics/PhysConfig.hpp>

class PhysicsSystem : public ISystem
{
public:
    ~PhysicsSystem();

public:
    void init(EngineContext* context) override;
    void update(Scene& scene, float dt) override;

    physx::PxTriangleMesh* cookTriangleMesh(const physx::PxTriangleMeshDesc& meshDesc);

    void createCharacterController(Scene& scene, Entity e);

    void moveCharacter(Scene& scene, Entity e, const glm::vec3& movement, float dt);

    PxPhysics* getPhysics() { return context.physics; }
    PxScene* getScene() { return context.scene; }

    PhysicsContext& getContext() { return context; }

private:
    PhysicsContext context;
};

REGISTER_SYSTEM(PhysicsSystem);