#pragma once

#include "IRenderPass.hpp"

#include <ecs/Scene.hpp>
#include <render/RenderContext.hpp>
#include <render/shader/Shader.hpp>
#include <ecs/components/Transform.hpp>

#include <ecs/components/ModelRendererComponent.hpp>
#include <render/renderer/IRenderDevice.hpp>
#include <render/renderer/RenderGraphStorage.hpp>
#include <render/renderer/gpu/GPULight.hpp>
#include <render/renderer/gpu/ShadowMap.hpp>
#include <vector>

class ScenePass : public IRenderPass
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
    void bindShadowMaps(Shader& shader, const std::vector<GPULight>& lights);

    static constexpr int MAX_LIGHTS = 8;
    static constexpr int SHADOW_TEXTURE_UNIT_START = 1;

    IRenderDevice* device = nullptr;
    IRenderPlatform* platform = nullptr;

    bool renderShadow = false;
    bool renderLights = false;

    int lightsHandleId;
    int shadowMapsHandleId;
};