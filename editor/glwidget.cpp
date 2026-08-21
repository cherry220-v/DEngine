#include "GLWidget.h"

#include <render/renderer/gl/OpenGLRenderDevice.hpp>
#include <core/platform/render/qt6/QtRenderPlatform.hpp>

GLWidget::GLWidget(IRenderer* renderer, Application* app, QWidget* parent)
    : QOpenGLWidget(parent),
    renderer(renderer),
    app(app)
{
    setFocusPolicy(Qt::StrongFocus);

    connect(
        &m_timer,
        &QTimer::timeout,
        this,
        QOverload<>::of(&QOpenGLWidget::update)
    );

    m_timer.start(16);
}

void GLWidget::initializeGL()
{
    emit initialized();
    isInitialized = true;
}

void GLWidget::paintGL()
{
    if (!app || !app->input())
        return;

    app->engine()->tick(app->input());
}

void GLWidget::resizeGL(int w, int h)
{
}

void GLWidget::resizeEvent(QResizeEvent* event)
{
    QOpenGLWidget::resizeEvent(event);

    resizeGL(width(), height());
}