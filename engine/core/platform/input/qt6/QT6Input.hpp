#pragma once

#include <core/platform/window/qt6/QT6Window.hpp>
#include "../InputSystem.hpp"

class QKeyEvent;
class QMouseEvent;
class QMainWindow;
class Qt6Window;

class QT6Input : public InputSystem
{
private:
    QMainWindow* m_qtwindow = nullptr;
    QT6Window* m_window = nullptr;

public:
    QT6Input(QT6Window* window);
    ~QT6Input() override = default;

    IWindow* window() const override { return this->m_window; }

    void update() override;

    void processKeyPress(QKeyEvent* event);
    void processKeyRelease(QKeyEvent* event);
    void processMouseMove(QMouseEvent* event);
    void processMousePress(QMouseEvent* event);
    void processMouseRelease(QMouseEvent* event);

private:
    Key mapKey(int qtKey) const;
    MouseButton mapMouse(int qtButton) const;
};
