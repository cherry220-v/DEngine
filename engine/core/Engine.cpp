#include "Engine.hpp"

#include "Application.h"

#include <render/renderer/passes/ShadowPass.hpp>
#include <render/renderer/passes/ScenePass.hpp>
#include <render/renderer/passes/SelectPass.hpp>
#include <render/renderer/passes/ParticlePass.hpp>
#include <render/renderer/passes/LightPass.hpp>
#include <render/renderer/passes/EditorPass.hpp>
#include <render/renderer/passes/OverlayPass.hpp>
#include <ecs/Scene.hpp>
#include <mutex>
#include <stdexcept>

void Engine::update(float dt, IInput* input)
{
    if (!scene || !input) return;
    scene->updateSystems(*scene, dt);
    //auto json = scene->serialize();

    //std::ofstream file("scene.json");
    //file << json.dump(4);
}

void Engine::render(RenderContext& context)
{
    if (!scene || !activeCamera)
        return;

    double currentTime = getTime();
    frameCount++;

    if (currentTime - fpsLastTime >= 1.0)
    {
        fps = frameCount / (currentTime - fpsLastTime);

        frameCount = 0;
        fpsLastTime = currentTime;
    }

    m_renderer->renderFrame(context);
}

void Engine::setRenderer(std::unique_ptr<Renderer> renderer)
{
    this->m_renderer = std::move(renderer);
}

void Engine::setScene(std::unique_ptr<Scene> s)
{
    scene = std::move(s);
}

Scene* Engine::getScene()
{
    return scene.get();
}

void Engine::setPlayerEntity(Entity player)
{
    playerEntity = player;
}

void Engine::setActiveCamera(Entity e)
{
    activeCamera = e;
}

void Engine::tick(IInput* input)
{
    if (!m_sceneInitialized)
    {
        scene->initSystems(context());
        m_sceneInitialized = true;
    }

    double current = getTime();
    double dt = current - lastTime;
    lastTime = current;

    accumulator += dt;

    input->update();

    while (accumulator >= fixedStep)
    {
        update(fixedStep, input);
        accumulator -= fixedStep;
    }

    RenderContext context;
    context.scene = scene.get();
    context.activeCamera = activeCamera;

    context.input = m_app->input();
    context.platform = m_renderer->getPlatform();
	context.deltaTime = static_cast<float>(dt);
    render(context);

    double now = getTime();
    double nextFrame = lastTime + frameTime;

    if (now < nextFrame)
    {
        std::this_thread::sleep_for(
            std::chrono::duration<double>(nextFrame - now)
        );
    }
}

void Engine::setupRenderer()
{
    auto editorPass = std::make_unique<EditorPass>();
    auto lightPass = std::make_unique<LightPass>();
    auto shadowPass = std::make_unique<ShadowPass>();
    auto scenePass = std::make_unique<ScenePass>();
    auto particlePass = std::make_unique<ParticlePass>();
    auto selectPass = std::make_unique<SelectPass>();
    auto overlayPass = std::make_unique<OverlayPass>();

    bool USE_EDITOR = 0;

    if (USE_EDITOR)
    {
        lightPass->after(*editorPass);
        m_renderer->graph.addPass(std::move(editorPass));
    }

    shadowPass->after(*lightPass);
    scenePass->after(*shadowPass);
    particlePass->after(*scenePass);
    //selectPass->after(*scenePass);
    overlayPass->after(*scenePass);

    m_renderer->graph.addPass(std::move(lightPass));
    m_renderer->graph.addPass(std::move(shadowPass));
    m_renderer->graph.addPass(std::move(scenePass));
    m_renderer->graph.addPass(std::move(particlePass));
    //m_renderer->graph.addPass(std::move(selectPass));
    m_renderer->graph.addPass(std::move(overlayPass));

    m_renderer->init();
}

EngineContext* Engine::context()
{
    EngineContext* ctx = new EngineContext
    {
        //.app = m_app,
        .engine = this,
        .renderer = m_renderer.get(),
        .input = m_app->input(),
        .window = m_app->window(),
        //.assets = &AssetManager::get()
    };
    return ctx;
}
