#pragma once

#include <windows.h>
#include "../InputSystem.hpp"

class Win32Window;

class Win32Input : public InputSystem
{
private:
    HWND m_hwnd = nullptr;
    IWindow* m_window = nullptr;

public:
    Win32Input(IWindow* window);
    bool processMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    ~Win32Input() override = default;

    void dXp(double x) { deltaX += x; };
    void dYp(double y) { deltaY += y; };

    IWindow* window() const override { return this->m_window; }
    void update() override;

private:
    Key mapKey(int win32Key) const;
    MouseButton mapMouse(int win32Button) const;
};
