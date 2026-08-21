#pragma once

#include "platform/window/IWindow.hpp"
#include <render/renderer/IRenderer.hpp>
#include <ecs/Scene.hpp>
#include <ecs/entity/Entity.hpp>

#include "Engine.hpp"

#include <memory>
#include <chrono>
#include <ecs/systems/PlayerSystem.hpp>

class Application
{
    PlayerSystem* playerSystem = nullptr;
    std::unique_ptr<IWindow> m_window;
    std::unique_ptr<IInput> m_input;
    std::unique_ptr<Engine> m_engine;

    double fps = 0.0;
    double lastTime = 0.0;

public:
    Application();

    void setWindow(std::unique_ptr<IWindow> window);
    void setInput(std::unique_ptr<IInput> input);

    IInput* input() { return m_input.get(); };
    IWindow* window() { return m_window.get(); };

    Engine* engine() { return m_engine.get(); }

    void run();
};