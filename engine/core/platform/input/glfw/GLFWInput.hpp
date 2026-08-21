#pragma once

#include <core/platform/window/glfw/GLFWWindow.hpp>
#include "../InputSystem.hpp"

class GLFWInput : public InputSystem
{
private:
    GLFWwindow* gwindow = nullptr;
    GLFWWindow* m_window = nullptr;

public:
    GLFWInput(GLFWWindow* window);
    ~GLFWInput() override = default;

    IWindow* window() const override { return this->m_window; }
    void update() override;

private:
    Key mapKey(int glfwKey) const;
    MouseButton mapMouse(int glfwButton) const;
};
