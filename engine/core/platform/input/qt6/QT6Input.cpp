#include "QT6Input.hpp"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QMainWindow>

QT6Input::QT6Input(QT6Window* win)
    : InputSystem(win), m_window(win)
{
    this->m_qtwindow = win->window;
}

void QT6Input::update()
{
    InputSystem::update();

    deltaX = mouseX - lastMouseX;
    deltaY = mouseY - lastMouseY;
}

void QT6Input::processKeyPress(QKeyEvent* event)
{
    if (event->isAutoRepeat()) return;

    Key key = mapKey(event->key());
    if (key != Key::Unknown)
    {
        current.set(static_cast<size_t>(key));
    }
}

void QT6Input::processKeyRelease(QKeyEvent* event)
{
    if (event->isAutoRepeat()) return;

    Key key = mapKey(event->key());
    if (key != Key::Unknown)
    {
        current.reset(static_cast<size_t>(key));
    }
}

void QT6Input::processMouseMove(QMouseEvent* event)
{
    mouseX = event->position().x();
    mouseY = event->position().y();
}

void QT6Input::processMousePress(QMouseEvent* event)
{
    MouseButton btn = mapMouse(static_cast<int>(event->button()));
    mouseCurrent.set(static_cast<size_t>(btn));
}

void QT6Input::processMouseRelease(QMouseEvent* event)
{
    MouseButton btn = mapMouse(static_cast<int>(event->button()));
    mouseCurrent.reset(static_cast<size_t>(btn));
}

Key QT6Input::mapKey(int key) const
{
    if (key >= Qt::Key_A && key <= Qt::Key_Z)
        return static_cast<Key>(static_cast<int>(Key::A) + (key - Qt::Key_A));

    if (key >= Qt::Key_0 && key <= Qt::Key_9)
        return static_cast<Key>(static_cast<int>(Key::Num0) + (key - Qt::Key_0));

    if (key >= Qt::Key_F1 && key <= Qt::Key_F24)
        return static_cast<Key>(static_cast<int>(Key::F1) + (key - Qt::Key_F1));

    switch (key)
    {
    case Qt::Key_Escape:    return Key::Escape;
    case Qt::Key_Return:    return Key::Enter;
    case Qt::Key_Enter:     return Key::Enter; // На нумпаде
    case Qt::Key_Tab:       return Key::Tab;
    case Qt::Key_Backspace: return Key::Backspace;
    case Qt::Key_Insert:    return Key::Insert;
    case Qt::Key_Delete:    return Key::Delete;
    case Qt::Key_Home:      return Key::Home;
    case Qt::Key_End:       return Key::End;
    case Qt::Key_PageUp:    return Key::PageUp;
    case Qt::Key_PageDown:  return Key::PageDown;
    case Qt::Key_Left:      return Key::Left;
    case Qt::Key_Right:     return Key::Right;
    case Qt::Key_Up:        return Key::Up;
    case Qt::Key_Down:      return Key::Down;
    case Qt::Key_Shift:     return Key::LeftShift; // И правый тоже
    case Qt::Key_Control:   return Key::LeftCtrl;
    case Qt::Key_Alt:       return Key::LeftAlt;
    case Qt::Key_Meta:      return Key::LeftSuper;
    case Qt::Key_CapsLock:  return Key::CapsLock;
    case Qt::Key_ScrollLock:return Key::ScrollLock;
    case Qt::Key_NumLock:   return Key::NumLock;
    case Qt::Key_Print:     return Key::PrintScreen;
    case Qt::Key_Pause:     return Key::Pause;
    case Qt::Key_Space:     return Key::Space;
    case Qt::Key_Menu:      return Key::Menu;
    default:                return Key::Unknown;
    }
}

MouseButton QT6Input::mapMouse(int qtButton) const
{
    switch (qtButton)
    {
    case Qt::LeftButton:   return MouseButton::Left;
    case Qt::RightButton:  return MouseButton::Right;
    case Qt::MiddleButton: return MouseButton::Middle;
    case Qt::XButton1:     return MouseButton::Button4;
    case Qt::XButton2:     return MouseButton::Button5;
    default:               return MouseButton::Left;
    }
}
