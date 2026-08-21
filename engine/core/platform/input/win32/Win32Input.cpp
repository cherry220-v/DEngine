#include "Win32Input.hpp"

Win32Input::Win32Input(IWindow* win)
    : InputSystem(win), m_window(win)
{
    this->m_hwnd = static_cast<HWND>(win->getNativeHandle());
}

bool Win32Input::processMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    {
        int vkCode = static_cast<int>(wParam);

        if (vkCode == VK_SHIFT) {
            vkCode = (((lParam >> 16) & 0xFF) == 0x36) ? VK_RSHIFT : VK_LSHIFT;
        }
        else if (vkCode == VK_CONTROL) {
            vkCode = (lParam & (1 << 24)) ? VK_RCONTROL : VK_LCONTROL;
        }
        else if (vkCode == VK_MENU) { // Alt
            vkCode = (lParam & (1 << 24)) ? VK_RMENU : VK_LMENU;
        }

        Key key = mapKey(vkCode);
        if (key != Key::Unknown) {
            current[static_cast<size_t>(key)] = true;
        }
        return true;
    }

    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        int vkCode = static_cast<int>(wParam);

        if (vkCode == VK_SHIFT) {
            vkCode = (((lParam >> 16) & 0xFF) == 0x36) ? VK_RSHIFT : VK_LSHIFT;
        }
        else if (vkCode == VK_CONTROL) {
            vkCode = (lParam & (1 << 24)) ? VK_RCONTROL : VK_LCONTROL;
        }
        else if (vkCode == VK_MENU) {
            vkCode = (lParam & (1 << 24)) ? VK_RMENU : VK_LMENU;
        }

        Key key = mapKey(vkCode);
        if (key != Key::Unknown) {
            current[static_cast<size_t>(key)] = false;
        }
        return true;
    }

    case WM_LBUTTONDOWN: mouseCurrent[static_cast<size_t>(MouseButton::Left)] = true;   return true;
    case WM_LBUTTONUP:   mouseCurrent[static_cast<size_t>(MouseButton::Left)] = false;  return true;
    case WM_RBUTTONDOWN: mouseCurrent[static_cast<size_t>(MouseButton::Right)] = true;  return true;
    case WM_RBUTTONUP:   mouseCurrent[static_cast<size_t>(MouseButton::Right)] = false; return true;
    case WM_MBUTTONDOWN: mouseCurrent[static_cast<size_t>(MouseButton::Middle)] = true; return true;
    case WM_MBUTTONUP:   mouseCurrent[static_cast<size_t>(MouseButton::Middle)] = false; return true;

    case WM_XBUTTONDOWN:
    {
        int xbtn = GET_XBUTTON_WPARAM(wParam);
        if (xbtn == XBUTTON1) mouseCurrent[static_cast<size_t>(MouseButton::Button4)] = true;
        if (xbtn == XBUTTON2) mouseCurrent[static_cast<size_t>(MouseButton::Button5)] = true;
        return true;
    }
    case WM_XBUTTONUP:
    {
        int xbtn = GET_XBUTTON_WPARAM(wParam);
        if (xbtn == XBUTTON1) mouseCurrent[static_cast<size_t>(MouseButton::Button4)] = false;
        if (xbtn == XBUTTON2) mouseCurrent[static_cast<size_t>(MouseButton::Button5)] = false;
        return true;
    }
    }
    return false;
}


Key Win32Input::mapKey(int key) const
{
    if (key >= 'A' && key <= 'Z')
        return static_cast<Key>(key);

    if (key >= '0' && key <= '9')
        return static_cast<Key>(key);

    if (key >= VK_F1 && key <= VK_F24)
        return static_cast<Key>(static_cast<int>(Key::F1) + (key - VK_F1));

    switch (key)
    {
        // Управляющие
    case VK_ESCAPE:    return Key::Escape;
    case VK_RETURN:
    {
        return Key::Enter;
    }
    case VK_TAB:       return Key::Tab;
    case VK_BACK:      return Key::Backspace;
    case VK_INSERT:    return Key::Insert;
    case VK_DELETE:    return Key::Delete;
    case VK_HOME:      return Key::Home;
    case VK_END:       return Key::End;
    case VK_PRIOR:     return Key::PageUp;
    case VK_NEXT:      return Key::PageDown;

        // Стрелки
    case VK_LEFT:      return Key::Left;
    case VK_RIGHT:     return Key::Right;
    case VK_UP:        return Key::Up;
    case VK_DOWN:      return Key::Down;

        // Модификаторы
    case VK_LSHIFT:    return Key::LeftShift;
    case VK_RSHIFT:    return Key::RightShift;
    case VK_LCONTROL:  return Key::LeftCtrl;
    case VK_RCONTROL:  return Key::RightCtrl;
    case VK_LMENU:     return Key::LeftAlt;
    case VK_RMENU:     return Key::RightAlt;
    case VK_LWIN:      return Key::LeftSuper;
    case VK_RWIN:      return Key::RightSuper;

        // Специальные
    case VK_CAPITAL:   return Key::CapsLock;
    case VK_SCROLL:    return Key::ScrollLock;
    case VK_NUMLOCK:   return Key::NumLock;
    case VK_SNAPSHOT:  return Key::PrintScreen;
    case VK_PAUSE:     return Key::Pause;
    case VK_SPACE:     return Key::Space;

        // Символы (Исправляет проблему, почему не нажимались знаки препинания на Windows)
    case VK_OEM_3:      return Key::GraveAccent;   // `
    case VK_OEM_MINUS:  return Key::Minus;         // -
    case VK_OEM_PLUS:   return Key::Equal;         // =
    case VK_OEM_4:      return Key::LeftBracket;   // [
    case VK_OEM_6:      return Key::RightBracket;  // ]
    case VK_OEM_5:      return Key::Backslash;     // \ *
    case VK_OEM_1:      return Key::Semicolon;     // ;
    case VK_OEM_7:      return Key::Apostrophe;    // '
    case VK_OEM_COMMA:  return Key::Comma;         // ,
    case VK_OEM_PERIOD: return Key::Period;        // .
    case VK_OEM_2:      return Key::Slash;         // /

        // Numpad
    case VK_NUMPAD0:   return Key::KP0;
    case VK_NUMPAD1:   return Key::KP1;
    case VK_NUMPAD2:   return Key::KP2;
    case VK_NUMPAD3:   return Key::KP3;
    case VK_NUMPAD4:   return Key::KP4;
    case VK_NUMPAD5:   return Key::KP5;
    case VK_NUMPAD6:   return Key::KP6;
    case VK_NUMPAD7:   return Key::KP7;
    case VK_NUMPAD8:   return Key::KP8;
    case VK_NUMPAD9:   return Key::KP9;

    case VK_DECIMAL:   return Key::KPDecimal;
    case VK_DIVIDE:    return Key::KPDivide;
    case VK_MULTIPLY:  return Key::KPMultiply;
    case VK_SUBTRACT:  return Key::KPSubtract;
    case VK_ADD:       return Key::KPAdd;

    case VK_APPS:      return Key::Menu;

    default:           return Key::Unknown;
    }
}


MouseButton Win32Input::mapMouse(int win32Button) const
{
    switch (win32Button)
    {
    case VK_LBUTTON:  return MouseButton::Left;
    case VK_RBUTTON:  return MouseButton::Right;
    case VK_MBUTTON:  return MouseButton::Middle;
    case VK_XBUTTON1: return MouseButton::Button4;
    case VK_XBUTTON2: return MouseButton::Button5;
    default:          return MouseButton::Left;
    }
}

void Win32Input::update()
{
    InputSystem::update();

    //deltaX = 0;
    //deltaY = 0;
}