#include "WirePass.hpp"
#include "ScenePass.hpp"

WirePass::WirePass(ScenePass* scenePass) : scenePass(scenePass)
{
}

void WirePass::init(
    IRenderPlatform* platform,
    IRenderDevice* device)
{
    this->platform = platform;
    this->device = device;
}

void WirePass::execute(const RenderContext& context, RenderGraphStorage& graph)
{
    if (!scenePass)
        return;

    device->setPolygonMode(PolygonMode::Line);

    device->setCullEnabled(false);

    scenePass->execute(context, graph);

    device->setCullEnabled(true);

    device->setPolygonMode(PolygonMode::Fill);
}