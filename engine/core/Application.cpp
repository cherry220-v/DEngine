#include "Application.h"

#include <ecs/components/ModelRendererComponent.hpp>
#include <ecs/systems/PlayerSystem.hpp>
#include <ecs/systems/CameraSystem.hpp>
#include <ecs/components/PlayerComponent.hpp>

#include <iostream>
#include <chrono>
#include <thread>

Application::Application()
{
    m_engine = std::make_unique<Engine>(this);
}

void Application::setWindow(std::unique_ptr<IWindow> window)
{
    this->m_window = std::move(window);
}

void Application::setInput(std::unique_ptr<IInput> input)
{
    this->m_input = std::move(input);
}

void Application::run()
{
    while (!m_window->shouldClose())
    {
        m_window->pollEvents();
        m_engine->tick(m_input.get());
        m_window->swapBuffers();
        //std::cout << (int)engine()->getFPS() << std::endl;
    }
}