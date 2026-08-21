#pragma once
#include <QMainWindow>
#include <core/platform/window/IWindow.hpp>
#include <QKeyEvent>

class QT6Input;

class QT6Window : public IWindow {
public:
    QT6Window(WindowProps& props, QMainWindow* window);

public:
    QMainWindow* window = nullptr;

    int getWidth() override;
    int getHeight() override;

    void pollEvents() override;
    void swapBuffers() override;
    bool shouldClose() const override;

    void* getNativeHandle() override;
    QMainWindow* gwindow() { return window; }

    void makeContextCurrent() override;

    void setCursorMode(CursorMode mode) override;
    CursorMode getCursorMode() override;

protected:
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
private:
    CursorMode currentMode = CursorMode::Normal;
};