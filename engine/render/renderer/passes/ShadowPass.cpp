#include "ShadowPass.hpp"

#include <math/ShadowMath.h>

#include <glad/glad.h>
#include <ecs/Scene.hpp>
#include <ecs/components/Transform.hpp>
#include <ecs/components/LightComponent.hpp>
#include <ecs/components/ModelRendererComponent.hpp>
#include <assets/AssetManager.hpp>
#include <format>

void ShadowPass::init(IRenderPlatform* platform, IRenderDevice* device)
{
    this->platform = platform;
    this->device = device;

    auto& assetManager = AssetManager::get();

    shadowMat = assetManager.load<Material>("50e6fbf1-4eee-4a81-b08f-14aabdbf16d5");
    shadowShader = shadowMat->shader;
}

void ShadowPass::setup(RenderGraphStorage& graph)
{
    shadowMapsHandleId = graph.create("ShadowPass.ShadowMaps");

    reads.clear();
    writes.clear();
    writes.push_back(shadowMapsHandleId);

    graph.publish("ShadowPass.ShadowMaps", shadowMapsHandleId);

    RenderHandle* lightsId = graph.import("LightPass.Lights");
    if (lightsId != nullptr) {
        reads.push_back(*lightsId);
        lightsHandleId = *lightsId;
        renderLights = true;
    }
}

void ShadowPass::execute(const RenderContext& ctx, RenderGraphStorage& graph)
{
    Scene* scene = ctx.scene;

    if (!renderLights)
    {
        graph.set(
            shadowMapsHandleId,
            std::make_shared<std::vector<ShadowMap>>()
        );
        return;
    }

    auto gpuLights =
        graph.get<std::vector<GPULight>>(lightsHandleId);

    auto* cameraTransform = scene->getComponent<Transform>(ctx.activeCamera);
    if (!cameraTransform)
        return;

    glm::vec3 cameraPos = cameraTransform->position;

    auto view = scene->view<ModelRendererComponent, Transform>();

    std::vector<GPULight> sortedLights;

    for (auto& light : *gpuLights)
    {
        if (static_cast<int>(light.params.x) == static_cast<int>(LightCType::Directional))
            sortedLights.push_back(light);
    }

    while (shadowPool.size() < sortedLights.size())
    {
        ShadowMap sm;
        sm.fbo = device->createFramebuffer();
        sm.texture = device->createDepthTexture(
            SHADOW_WIDTH,
            SHADOW_HEIGHT
        );

        device->attachDepthTexture(
            sm.fbo,
            sm.texture
        );
        shadowPool.push_back(sm);
    }

    int shadowIndex = 0;
    for (auto& light : sortedLights)
    {
        ShadowMap& sm = shadowPool.at(shadowIndex++);

        light.lightSpaceMatrix =
            createDirectionalLightSpaceMatrix(
                glm::normalize(glm::vec3(light.direction)),
                cameraPos,
                50.0f,
                1.0f,
                200.0f
            );

        device->setViewport(SHADOW_WIDTH, SHADOW_HEIGHT);
        device->bindFramebuffer(sm.fbo);
        device->clearDepth();

        shadowShader->use();
        shadowShader->setMat4("lightSpaceMatrix", light.lightSpaceMatrix);

        for (auto e : view)
        {
            auto* renderer = scene->getComponent<ModelRendererComponent>(e);
            auto* transform = scene->getComponent<Transform>(e);

            if (!renderer->model)
                continue;

            shadowShader->setMat4("model", scene->getWorldMatrix(e));
            renderer->model.get()->draw();
        }

        light.params.y = sm.texture;
    }

    //graph.set(
    //    lightsHandleId,
    //    std::make_shared<std::vector<GPULight>>(sortedLights)
    //);

    graph.set(
        shadowMapsHandleId,
        std::make_shared<std::vector<ShadowMap>>(shadowPool)
    );
}