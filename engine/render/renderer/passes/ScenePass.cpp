#include "ScenePass.hpp"
#include <math/CameraMath.h>
#include <ecs/Scene.hpp>
#include <ecs/components/LightComponent.hpp>
#include <ecs/components/ColliderComponent.hpp>
#include <ecs/components/RigidBodyComponent.hpp>
#include <physx/PxActor.h>

#include <algorithm>
#include <PxPhysicsAPI.h>
#include <physx/geometry/PxTriangleMesh.h>
#include <ecs/systems/PhysicsSystem.hpp>
#include <ecs/components/MeshColliderComponent.hpp>

#include <glm/glm.hpp>

void ScenePass::init(IRenderPlatform* platform, IRenderDevice* device)
{
    this->platform = platform;
    this->device = device;
}

void ScenePass::setup(RenderGraphStorage& graph)
{
    reads.clear();
    renderShadow = false;
    renderLights = false;

    RenderHandle* shadowMapsId = graph.import("ShadowPass.ShadowMaps");
    RenderHandle* lightsId = graph.import("LightPass.Lights");
    if (shadowMapsId != nullptr) {
        reads.push_back(*shadowMapsId);
        shadowMapsHandleId = *shadowMapsId;
        renderShadow = true;
    }
    if (lightsId != nullptr) {
        reads.push_back(*lightsId);
        lightsHandleId = *lightsId;
        renderLights = true;
    }
}

void ScenePass::execute(const RenderContext& context, RenderGraphStorage& graph)
{
    Scene* scene = context.scene;

    auto* camTransform = scene->getComponent<Transform>(context.activeCamera);
    auto* cam = scene->getComponent<CameraComponent>(context.activeCamera);

    if (!cam || !camTransform) return;

    device->bindFramebuffer(platform->getDefaultFBO());

    const std::shared_ptr<std::vector<GPULight>> lights =
        renderLights ? graph.get<std::vector<GPULight>>(lightsHandleId) : nullptr;

    for (auto e : scene->view<ModelRendererComponent, Transform>())
    {
        auto* renderer = scene->getComponent<ModelRendererComponent>(e);
        auto* transform = scene->getComponent<Transform>(e);

        if (!renderer->model || !renderer->material)
            continue;

        auto rendererModel = renderer->model.get();
        auto rendererMaterial = renderer->material.get();

        auto& shader = rendererMaterial->shader;
        rendererMaterial->bind();
        shader->setMat4("view", getViewMatrix(*camTransform));
        shader->setMat4("projection", getProjectionMatrix(*cam));
        shader->setVec3("viewPos", camTransform->position);

        shader->setFloat("shininess", rendererMaterial->shininess);
        shader->setFloat("specularStrength", rendererMaterial->specularStrength);
        shader->setVec4("objectColor", rendererMaterial->color);

        if (lights)
        {
            uploadLights(*shader, *lights);
            for (int i = 0; i < MAX_LIGHTS; i++)
                shader->setInt("hasShadowMap[" + std::to_string(i) + "]", 0);
            bindShadowMaps(*shader, *lights);
        }
        else
        {
            shader->setInt("lightCount", 0);
        }

        if (scene->hasComponent<MeshColliderComponent>(e))
        {
            auto* comp = scene->getComponent<MeshColliderComponent>(e);
            auto* actor = comp->actor;
            if (!actor) continue;
            physx::PxTransform pxTransform = actor->getGlobalPose();

            glm::quat glmRot;
            glmRot.w = pxTransform.q.w;
            glmRot.x = pxTransform.q.x;
            glmRot.y = pxTransform.q.y;
            glmRot.z = pxTransform.q.z;

            glm::mat4 physModelMat = glm::translate(glm::mat4(1.0f), glm::vec3(pxTransform.p.x, pxTransform.p.y, pxTransform.p.z)) *
                glm::mat4_cast(glmRot);

            glm::mat4 newWorldMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(pxTransform.p.x, pxTransform.p.y, pxTransform.p.z)) * glm::mat4_cast(glmRot) * glm::scale(glm::mat4(1.0f), transform->scale);

            if (scene->getSceneGraph().getParent(e))
            {
                glm::mat4 parentWorldInv = glm::inverse(scene->getComponent<Transform>(scene->getSceneGraph().getParent(e))->worldMatrix);
                transform->updateFromMatrix(parentWorldInv * newWorldMatrix);
            }
            else {
                transform->updateFromMatrix(newWorldMatrix);
            }

            auto* renderer = scene->getComponent<ModelRendererComponent>(e);
            glm::vec3 scale, pos, skew; glm::quat rot; glm::vec4 pers;
            glm::decompose(scene->getWorldMatrix(e), scale, rot, pos, skew, pers);
            
            auto modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(pxTransform.p.x, pxTransform.p.y, pxTransform.p.z)) *
                glm::mat4_cast(glmRot) *
                glm::scale(glm::mat4(1.0f), transform->scale);

            shader->setMat4("model", modelMatrix);
        }
        else
        {
            shader->setMat4("model", scene->getWorldMatrix(e));
        }

        rendererModel->draw();
    }
}

void ScenePass::bindShadowMaps(Shader& shader, const std::vector<GPULight>& lights)
{
    if (!renderShadow)
        return;

    const int count = std::min(static_cast<int>(lights.size()), MAX_LIGHTS);

    for (int i = 0; i < count; i++)
    {
        const auto& light = lights[i];
        if (static_cast<int>(light.params.x) != static_cast<int>(LightCType::Directional))
            continue;

        const unsigned int shadowTexture = static_cast<unsigned int>(light.params.y);
        if (shadowTexture == 0)
            continue;

        const int textureUnit = SHADOW_TEXTURE_UNIT_START + i;
        device->bindTexture2D(shadowTexture, textureUnit);

        const auto index = std::to_string(i);
        shader.setMat4("lightSpaceMatrices[" + index + "]", light.lightSpaceMatrix);
        shader.setInt("shadowMaps[" + index + "]", textureUnit);
        shader.setInt("hasShadowMap[" + index + "]", 1);
    }
}