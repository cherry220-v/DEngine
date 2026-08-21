#pragma once

#include "ecs/Scene.hpp"
#include <vector>
#include "../RenderResource.hpp"

class RenderGraphStorage;
class IRenderPlatform;
class IRenderDevice;

struct RenderContext;

class IRenderPass
{
public:
    virtual ~IRenderPass() = default;

    virtual void init(
        IRenderPlatform* platform,
        IRenderDevice* device) = 0;

    virtual void setup(
        RenderGraphStorage& graph)
    {
    }

    virtual void execute(
        const RenderContext& ctx,
        RenderGraphStorage& graph) = 0;

    void after(IRenderPass& pass)
    {
        afterPasses.push_back(&pass);
    }

    std::vector<RenderHandle> writes;
    std::vector<RenderHandle> reads;
    std::vector<IRenderPass*> afterPasses;
};