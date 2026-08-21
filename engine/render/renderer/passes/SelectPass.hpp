#pragma once

#include "IRenderPass.hpp"

#include <ecs/Scene.hpp>
#include <render/RenderContext.hpp>
#include <render/shader/Shader.hpp>
#include <ecs/components/Transform.hpp>

#include <ecs/components/ModelRendererComponent.hpp>
#include <render/renderer/IRenderDevice.hpp>
#include <render/renderer/RenderGraphStorage.hpp>

class SelectPass : public IRenderPass
{
public:
    void init(IRenderPlatform* platform, IRenderDevice* device) override;

    void setup(RenderGraphStorage& graph) override;

    void execute(const RenderContext& ctx, RenderGraphStorage& graph) override;

    unsigned int pickObject(int mouseX, int mouseY)
    {
        device->bindFramebuffer(selectionFBO);

        unsigned int id = 0;

        device->readPixels(mouseX, platform->getHeight() - mouseY);

        device->bindFramebuffer(platform->getDefaultFBO());
        device->setViewport(platform->getWidth(), platform->getHeight());
        return id;
    }
private:
    std::shared_ptr<Shader> selectionShader;
    IRenderDevice* device = nullptr;
    IRenderPlatform* platform = nullptr;

    unsigned int selectionFBO = 0;
    unsigned int selectionMap = 0;

    int selectedEntityHandleId;

};