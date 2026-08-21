#pragma once

#include "platform/input/IInput.hpp"

#include "EngineContext.hpp"

#include <ecs/systems/PlayerSystem.hpp>
#include <ecs/systems/LightSystem.hpp>
#include <ecs/systems/ParticleSystem.hpp>
#include <ecs/systems/SceneGraphSystem.hpp>
#include <ecs/systems/PhysicsSystem.hpp>

#include <ecs/components/PlayerComponent.hpp>
#include <ecs/components/Transform.hpp>
#include <ecs/entity/Entity.hpp>
#include <ecs/Scene.hpp>

#include <render/renderer/Renderer.hpp>
#include <render/renderer/RenderGraph.hpp>
#include <render/renderer/passes/IRenderPass.hpp>

#include <chrono>

class Application;

inline double getTime()
{
    static auto start = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = now - start;
    return diff.count();
}

class Engine
{
public:
    Engine(Application* app) : m_app(app) {}

public:
    void update(float dt, IInput* input);
    void render(RenderContext& context);

    void setScene(std::unique_ptr<Scene> scene);
    Scene* getScene();
    void setRenderer(std::unique_ptr<Renderer> renderer);

    void setPlayerEntity(Entity player);
    void setActiveCamera(Entity e);

    void tick(IInput* input);

    void setupRenderer();

    double getFPS() { return fps; }
    IRenderer* getRenderer() { return m_renderer.get(); }

    EngineContext* context();

private:
    std::unique_ptr<Scene> scene;
    Entity activeCamera = -1;

    Entity playerEntity = -1;

    std::unique_ptr<Renderer> m_renderer = nullptr;

    Application* m_app;

    bool m_sceneInitialized = false;

    // FPS

    double accumulator = 0.0;
    double fixedStep = 1.0 / 120.0;
    double lastTime = 0.0;

    const double targetFPS = 120.0;
    double frameTime = 1.0 / targetFPS;
    double fps = 0.0;
    int frameCount = 0;
    double fpsLastTime = 0.0;

};