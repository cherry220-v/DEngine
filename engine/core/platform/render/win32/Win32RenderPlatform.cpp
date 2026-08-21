#include "Win32RenderPlatform.hpp"
#include <core/platform/window/IWindow.hpp>
#include <core/platform/window/win32/Win32Window.hpp>
#include <stdexcept>

Win32RenderPlatform::Win32RenderPlatform(IWindow* window) : m_window(window) {
    if (!window) throw std::runtime_error("Window cannot be null!");

    m_hwnd = static_cast<HWND>(window->getNativeHandle());
    m_hdc = GetDC(m_hwnd);

    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(m_hdc, &pfd);
    if (!pixelFormat) throw std::runtime_error("Failed to choose pixel format!");

    if (!SetPixelFormat(m_hdc, pixelFormat, &pfd)) {
        throw std::runtime_error("Failed to set pixel format!");
    }

    m_hglrc = wglCreateContext(m_hdc);
    if (!m_hglrc) throw std::runtime_error("Failed to create OpenGL context!");
}

Win32RenderPlatform::~Win32RenderPlatform() {
    if (wglGetCurrentContext() == m_hglrc) {
        wglMakeCurrent(nullptr, nullptr);
    }
    if (m_hglrc) wglDeleteContext(m_hglrc);
    if (m_hdc) ReleaseDC(m_hwnd, m_hdc);
}

void Win32RenderPlatform::makeCurrent() {
    wglMakeCurrent(m_hdc, m_hglrc);
}

void Win32RenderPlatform::doneCurrent() {
    wglMakeCurrent(nullptr, nullptr);
}

void Win32RenderPlatform::swapBuffers() {
    SwapBuffers(m_hdc);
}

void* Win32RenderPlatform::getProcAddress(const char* name) {
    void* p = (void*)wglGetProcAddress(name);
    if (p == 0 || (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) || (p == (void*)-1)) {
        HMODULE module = LoadLibraryA("opengl32.dll");
        p = (void*)GetProcAddress(module, name);
    }
    return p;
}

int Win32RenderPlatform::getWidth() const { return m_window->getWidth(); }
int Win32RenderPlatform::getHeight() const { return m_window->getHeight(); }
