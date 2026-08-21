#pragma once
#include "IRenderPass.hpp"

#include <ecs/Scene.hpp>
#include <render/RenderContext.hpp>
#include <render/shader/Shader.hpp>
#include <ecs/components/Transform.hpp>

#include <ecs/components/ModelRendererComponent.hpp>
#include <render/renderer/IRenderDevice.hpp>

class ScenePass;

class WirePass : public IRenderPass
{
public:
    WirePass(ScenePass* pass);
    void init(IRenderPlatform* platform, IRenderDevice* device) override;

    void execute(
        const RenderContext& ctx,
        RenderGraphStorage& graph
    ) override;

private:
    IRenderDevice* device = nullptr;
    IRenderPlatform* platform = nullptr;

    ScenePass* scenePass = nullptr;
};