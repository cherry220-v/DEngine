#pragma once

#include <glad/glad.h>

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QResizeEvent>
#include <QTimer>
#include <QElapsedTimer>

#include <core/platform/render/qt6/QtRenderPlatform.hpp>
#include <core/platform/input/qt6/QT6Input.hpp>
#include <core/Application.h>

#include <render/renderer/IRenderer.hpp>
#include <render/RenderContext.hpp>

class GLWidget : public QOpenGLWidget
{
    Q_OBJECT

signals:
    void initialized();

public:
    GLWidget(IRenderer* renderer, Application* app, QWidget* parent);

    void setContext(RenderContext& ctx) {
        this->context = &ctx;
    }

    void setApplication(Application* app) {
        this->app = app;
	}

    void setInput(QT6Input* input) {
        this->input = input;
    }

    QT6Input* getInput() {
        return this->input;
    }

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void resizeEvent(QResizeEvent* e) override;

private:
    QTimer m_timer;
    float deltaTime = 0.0f;

    Application* app;
    IRenderer* renderer;
    RenderContext* context = nullptr;
    QT6Input* input = nullptr;

    QPoint lastGlobalMousePos;
    bool isInitialized = false;
};