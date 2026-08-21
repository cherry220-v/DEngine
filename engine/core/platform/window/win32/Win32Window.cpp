#include "Win32Window.hpp"
#include <stdexcept>
#include <vector>

const char* g_ClassName = "DEngine_WindowClass";

Win32Window::Win32Window(const WindowProps& props)
    : m_width(props.width), m_height(props.height)
{
    HINSTANCE hInstance = GetModuleHandle(nullptr);

    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = Win32Window::WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = g_ClassName;

    RegisterClassExA(&wc);

    RECT rect = { 0, 0, m_width, m_height };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    m_hwnd = CreateWindowExA(
        0, g_ClassName, props.title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!m_hwnd) {
        throw std::runtime_error("Failed to create Win32 Window!");
    }

    RAWINPUTDEVICE rid = {};
    rid.usUsagePage = 0x01;
    rid.usUsage = 0x02;
    rid.dwFlags = 0; // <-- ВАЖНО (для теста)
    rid.hwndTarget = m_hwnd;

    RegisterRawInputDevices(&rid, 1, sizeof(rid));

    SetWindowLongPtrA(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    m_hdc = GetDC(m_hwnd);

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);
}

Win32Window::~Win32Window()
{
    if (m_hglrc) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(m_hglrc);
    }
    if (m_hdc) ReleaseDC(m_hwnd, m_hdc);
    if (m_hwnd) DestroyWindow(m_hwnd);
}

void Win32Window::pollEvents()
{
    MSG msg;
    while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

void Win32Window::swapBuffers()
{
    SwapBuffers(m_hdc);
}

void Win32Window::makeContextCurrent()
{
    wglMakeCurrent(m_hdc, m_hglrc);
}

void Win32Window::setCursorMode(CursorMode mode)
{
    m_cursorMode = mode;
    if (mode == CursorMode::Locked) {
        RECT rect;
        GetClientRect(m_hwnd, &rect);
        MapWindowPoints(m_hwnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
        ClipCursor(&rect);
        ShowCursor(FALSE);
    }
    else if (mode == CursorMode::Hidden) {
        ClipCursor(nullptr);
        ShowCursor(FALSE);
    }
    else {
        ClipCursor(nullptr);
        ShowCursor(TRUE);
    }
}

LRESULT CALLBACK Win32Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Win32Window* window = reinterpret_cast<Win32Window*>(GetWindowLongPtrA(hwnd, GWLP_USERDATA));

    if (window)
    {
        if (window->m_input)
        {
            if (window->m_input->processMessage(hwnd, msg, wParam, lParam)) {
                return 0;
            }
        }

        switch (msg)
        {
        case WM_SIZE:
            window->m_width = LOWORD(lParam);
            window->m_height = HIWORD(lParam);
            return 0;

        case WM_CLOSE:
            window->m_shouldClose = true;
            return 0;
        case WM_INPUT:
        {
            OutputDebugStringA("WM_INPUT\n");
            UINT size = 0;
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));

            std::vector<BYTE> buffer(size);

            if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer.data(), &size, sizeof(RAWINPUTHEADER)) == size)
            {
                RAWINPUT* raw = (RAWINPUT*)buffer.data();

                if (raw->header.dwType == RIM_TYPEMOUSE)
                {
                    window->m_input->dXp(raw->data.mouse.lLastX);
                    window->m_input->dYp(raw->data.mouse.lLastY);
                }
            }

            return true;
        }
        case WM_MOUSEMOVE:
        {
            OutputDebugStringA("WM_MOUSEMOVE\n");
            return 0;
        }
        }
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}
