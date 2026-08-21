#pragma once

#include <render/RenderContext.hpp>

#include "IRenderPass.hpp"
#include <render/renderer/IRenderDevice.hpp>
#include <render/renderer/RenderGraphStorage.hpp>
#include <render/renderer/gpu/GPULight.hpp>
#include <vector>

class LightPass : public IRenderPass
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
    IRenderDevice* device = nullptr;
    IRenderPlatform* platform = nullptr;

    int lightsHandleId;
};