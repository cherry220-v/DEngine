#pragma once
#include "../IWindow.hpp"
#include <core/platform/input/win32/Win32Input.hpp>
#include <windows.h>

class Win32Window : public IWindow
{
public:
    Win32Window(const WindowProps& props);
    ~Win32Window() override;

    int getWidth() override { return m_width; }
    int getHeight() override { return m_height; }

    void pollEvents() override;
    void swapBuffers() override;
    bool shouldClose() const override { return m_shouldClose; }

    void* getNativeHandle() override { return static_cast<void*>(m_hwnd); }

    void makeContextCurrent() override;

    void setCursorMode(CursorMode mode) override;
    CursorMode getCursorMode() override { return m_cursorMode; }

    void setInputSystem(Win32Input* input) { m_input = input; }

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    HWND m_hwnd = nullptr;
    HDC m_hdc = nullptr;
    HGLRC m_hglrc = nullptr;

    int m_width = 0;
    int m_height = 0;
    bool m_shouldClose = false;

    CursorMode m_cursorMode = CursorMode::Normal;
    Win32Input* m_input = nullptr;
};
