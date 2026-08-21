#pragma once

#include "../IWindow.hpp"

class GLFWWindow : public IWindow {
private:
    void init(WindowProps& props);
    void shutdown();
private:
    struct GLFWwindow* window = nullptr;
    WindowProps* props = nullptr;

public:
    GLFWWindow(int width, int height, const char* title);
    GLFWWindow(WindowProps& props);
    ~GLFWWindow();

    int getWidth() override;
    int getHeight() override;

    void pollEvents() override;
    void swapBuffers() override;
    bool shouldClose() const override;

    void* getNativeHandle() override;
    GLFWwindow* gwindow() { return window; }

    void makeContextCurrent() override;

    void setCursorMode(CursorMode mode) override;
    CursorMode getCursorMode() override;

    static void resizeEvent(GLFWwindow* window, int w, int h);
};