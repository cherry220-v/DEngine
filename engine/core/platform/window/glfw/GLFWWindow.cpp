#include "GLFWWindow.hpp"

#include <GLFW/glfw3.h>
#include <stdexcept>

GLFWWindow::GLFWWindow(int width, int height, const char* title)
{
    this->props = new WindowProps{ width, height, title };
    init(*props);
}

GLFWWindow::GLFWWindow(WindowProps& props) : props(&props)
{
    init(props);
}

GLFWWindow::~GLFWWindow() {
    shutdown();
}

int GLFWWindow::getWidth() { int w, h; glfwGetWindowSize(window, &w, &h); return w; }
int GLFWWindow::getHeight() { int w, h; glfwGetWindowSize(window, &w, &h); return h; }

void GLFWWindow::init(WindowProps& props) {
    if (!glfwInit())
        throw std::runtime_error("Failed to init GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(props.width, props.height, props.title, nullptr, nullptr);

    if (!window)
        throw std::runtime_error("Failed to create GLFW window");

    glfwMakeContextCurrent(window);

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, &GLFWWindow::resizeEvent);
}

void GLFWWindow::shutdown() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void GLFWWindow::pollEvents() {
    glfwPollEvents();
}

void GLFWWindow::swapBuffers() {
    glfwSwapBuffers(window);
}

bool GLFWWindow::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void* GLFWWindow::getNativeHandle() {
    return window;
}

void GLFWWindow::makeContextCurrent() {
    glfwMakeContextCurrent(window);
}

void GLFWWindow::setCursorMode(CursorMode mode) {
    switch (mode)
    {
    case CursorMode::Normal: { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); break; }
    case CursorMode::Hidden: { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); break; }
    case CursorMode::Locked: { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); break; }

    }
}

CursorMode GLFWWindow::getCursorMode()
{
    auto mode = glfwGetInputMode(window, GLFW_CURSOR);
    switch (mode)
    {
    case GLFW_CURSOR_NORMAL: { return CursorMode::Normal; break; }
    case GLFW_CURSOR_HIDDEN: { return CursorMode::Hidden; break; }
    case GLFW_CURSOR_DISABLED: { return CursorMode::Locked; break; }
    }
}

void GLFWWindow::resizeEvent(GLFWwindow* window, int w, int h)
{
    auto self = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));

    if (self == nullptr || self->props == nullptr)
		return;

    self->props->width = w;
    self->props->height = h;
    glViewport(0, 0, w, h);
}
