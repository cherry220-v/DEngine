#include "Renderer.hpp"
#include <glad/glad.h>
#include <iostream>

#include <math/CameraMath.h>

Renderer::Renderer() : assetManager(AssetManager::get())
{
}

void Renderer::init()
{
    device->init(platform.get());

    graph.init(platform.get(), device.get());
    graph.build();
    glGenTextures(1, &m_tex);
}

void Renderer::captureFromFBO()
{
    device->bindTexture2D(m_tex);

    device->bindFramebuffer(platform->getDefaultFBO());

    glCopyTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        0, 0,
        0, 0,
        platform->getWidth(),
        platform->getHeight()
    );
}

void Renderer::renderFrame(
    const RenderContext& ctx)
{
    if (!ctx.scene) return;
    beginFrame();

    graph.execute(ctx);

    endFrame();
}

void Renderer::beginFrame()
{
    device->bindFramebuffer(platform->getDefaultFBO());

    device->setViewport(
        platform->getWidth(),
        platform->getHeight()
    );

    device->clearColor(
        1.0f,
        0.1f,
        0.1f,
        1.0f
    );
    device->clear();
}

void Renderer::endFrame()
{

}

void Renderer::setDevice(std::unique_ptr<IRenderDevice> device)
{
    this->device = std::move(device);
}

void Renderer::setPlatform(std::unique_ptr<IRenderPlatform> platform)
{
    this->platform = std::move(platform);
}

void Renderer::addPass(std::unique_ptr<IRenderPass> pass)
{
    graph.addPass(std::move(pass));
}