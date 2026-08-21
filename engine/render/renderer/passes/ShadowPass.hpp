#pragma once
#include "IRenderPass.hpp"

#include <ecs/Scene.hpp>
#include <render/RenderContext.hpp>
#include <render/shader/Shader.hpp>
#include <render/texture/Material.hpp>
#include <ecs/components/Transform.hpp>

#include <ecs/components/ModelRendererComponent.hpp>
#include <render/renderer/IRenderDevice.hpp>
#include <render/renderer/RenderGraphStorage.hpp>
#include <render/renderer/gpu/ShadowMap.hpp>
#include <render/renderer/gpu/GPULight.hpp>

class ShadowPass : public IRenderPass
{
public:
    void init(IRenderPlatform* platform, IRenderDevice* device) override;

    void setup(
        RenderGraphStorage& graph
    ) override;

    void execute(
        const RenderContext& ctx,
        RenderGraphStorage& graph
    ) override;

private:
    std::shared_ptr<Shader> shadowShader;
    std::shared_ptr<Material> shadowMat;
    IRenderDevice* device = nullptr;
    IRenderPlatform* platform = nullptr;

    int shadowMapsHandleId;
    std::vector<ShadowMap> shadowPool;

    unsigned int shadowFBO = 0;

    bool renderLights = false;
    int lightsHandleId;

    static constexpr int SHADOW_WIDTH = 2048;
    static constexpr int SHADOW_HEIGHT = 2048;

    static constexpr int MAX_SHADOWMAPS = 8;
};