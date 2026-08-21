#pragma once
#include <core/platform/render/IRenderPlatform.hpp>
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QPointer>
#include <QOpenGLFunctions>

class QtRenderPlatform : public IRenderPlatform {
    QWidget* m_window = nullptr;
    unsigned int m_fbo = 0;
public:
    QtRenderPlatform(QWidget* window)
        : m_window(window)
    { }

    void setDefaultFBO(unsigned int fbo)
    {
        m_fbo = fbo;
    }

    void makeCurrent() override
    {
    }

    void doneCurrent() override
    {
    }

    void swapBuffers() override
    {
    }

    void* getProcAddress(const char* name) override
    {
        auto* ctx = QOpenGLContext::currentContext();

        return ctx
            ? reinterpret_cast<void*>(ctx->getProcAddress(name))
            : nullptr;
    }

    unsigned int getDefaultFBO() const override
    {
        return m_fbo;
    }

    int getWidth() const override
    {
        return int(m_window->width() * m_window->devicePixelRatio());
    }

    int getHeight() const override
    {
        return int(m_window->height() * m_window->devicePixelRatio());
    }
};