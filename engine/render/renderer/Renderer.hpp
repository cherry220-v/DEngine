#pragma once
#include <core/platform/render/IRenderPlatform.hpp>

#include <render/RenderContext.hpp>
#include <render/renderer/IRenderer.hpp>
#include <render/renderer/IRenderDevice.hpp>
#include <render/renderer/passes/IRenderPass.hpp>

#include <assets/AssetManager.hpp>
#include <ecs/components/ModelRendererComponent.hpp>

#include <render/texture/Material.hpp>
#include <models/Model.hpp>

#include "RenderGraph.hpp"

class Renderer : public IRenderer {
public:
    Renderer();
    ~Renderer() = default;

    void init() override;
    void renderFrame(const RenderContext& ctx) override;

    void addPass(
        std::unique_ptr<IRenderPass> pass
    );

    void setPlatform(
        std::unique_ptr<IRenderPlatform> platform
    );

    IRenderPlatform* getPlatform() { return platform.get(); };

    void setDevice(
        std::unique_ptr<IRenderDevice> device
    );

    void beginFrame();
    void endFrame();

    void captureFromFBO();
    unsigned int texture() override { return m_tex; }

    void shutdown() override {
        platform->doneCurrent();
    };

private:
    std::unique_ptr<IRenderDevice> device;
    std::unique_ptr<IRenderPlatform> platform;

    AssetManager& assetManager;

    unsigned int m_tex = -1;
};
