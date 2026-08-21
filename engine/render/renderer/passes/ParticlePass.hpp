#pragma once

#include "IRenderPass.hpp"
#include <ecs/Scene.hpp>
#include <render/RenderContext.hpp>
#include <render/shader/Shader.hpp>
#include <ecs/components/Transform.hpp>

#include <ecs/components/ModelRendererComponent.hpp>
#include <render/renderer/IRenderDevice.hpp>
#include <render/renderer/RenderGraphStorage.hpp>

class ParticlePass : public IRenderPass
{
public:
    void init(IRenderPlatform* platform, IRenderDevice* device) override;

    void execute(
        const RenderContext& ctx,
        RenderGraphStorage& graph
    ) override;

private:
    std::shared_ptr<Shader> particleShader;
    std::shared_ptr<Material> particleMat;

    IRenderDevice* device = nullptr;
    IRenderPlatform* platform = nullptr;

    unsigned int quadVAO = 0;
    unsigned int quadVBO = 0;

    void createQuad();
};