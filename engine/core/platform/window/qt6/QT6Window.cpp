#include "QT6Window.hpp"
#include <core/platform/input/qt6/QT6Input.hpp>
#include <qmainwindow.h>
#include <QWidget>
#include <QCursor>

QT6Window::QT6Window(WindowProps& props, QMainWindow* window)
{
    this->window = window;

    this->window->show();
}

int QT6Window::getWidth() { return window->width(); }
int QT6Window::getHeight() { return window->height(); }

void QT6Window::pollEvents()
{
}

void QT6Window::swapBuffers()
{
}

bool QT6Window::shouldClose() const
{
    return false;
}

void* QT6Window::getNativeHandle()
{
    return nullptr;
}

void QT6Window::makeContextCurrent()
{
}

void QT6Window::setCursorMode(CursorMode mode)
{
    currentMode = mode;
    switch (mode)
    {
    case Normal: {
        window->releaseMouse();
        window->setCursor(Qt::ArrowCursor);
        window->setMouseTracking(true);
        break;
    }
    case Hidden: {
        window->releaseMouse();
        window->setCursor(Qt::BlankCursor);
        break;
    }
    case Locked: {
        window->setCursor(Qt::BlankCursor);
        window->grabMouse();
        break;
    }
    }
}

CursorMode QT6Window::getCursorMode()
{
    return currentMode;
}