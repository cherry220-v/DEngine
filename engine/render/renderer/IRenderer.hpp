#pragma once
#include "../RenderContext.hpp"
#include <core/platform/render/IRenderPlatform.hpp>
#include "RenderGraph.hpp"

class IRenderer {

public:
    virtual ~IRenderer() = default;

    virtual void init() = 0;

    virtual void renderFrame(const RenderContext& context) = 0;

    virtual void shutdown() = 0;

    virtual unsigned int texture() = 0;

    RenderGraph graph;
};
