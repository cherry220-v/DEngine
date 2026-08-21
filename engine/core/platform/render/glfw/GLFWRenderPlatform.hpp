#pragma once
#include <glad/glad.h>
#include <core/platform/render/IRenderPlatform.hpp>
#include <GLFW/glfw3.h>

class GLFWRenderPlatform : public IRenderPlatform {
    GLFWwindow* window;

public:
    GLFWRenderPlatform(GLFWwindow* w) : window(w) {}

    void makeCurrent() override {
        glfwMakeContextCurrent(window);
    }

    void doneCurrent() override {}

    void swapBuffers() override {
        glfwSwapBuffers(window);
    }

    void* getProcAddress(const char* name) override {
        return (void*)glfwGetProcAddress(name);
    }

    unsigned int getDefaultFBO() const override {
        return 0;
    }

    int getWidth() const override {
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        return w;
    }

    int getHeight() const override {
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        return h;
    }
};