#pragma once
#include "../IRenderPlatform.hpp"
#include <windows.h>

class IWindow;

class Win32RenderPlatform : public IRenderPlatform {
public:
    Win32RenderPlatform(IWindow* window);
    ~Win32RenderPlatform() override;

    void makeCurrent() override;
    void doneCurrent() override;
    void swapBuffers() override;
    void* getProcAddress(const char* name) override;

    unsigned int getDefaultFBO() const override { return 0; }

    int getWidth() const override;
    int getHeight() const override;

private:
    IWindow* m_window = nullptr;
    HWND m_hwnd = nullptr;
    HDC m_hdc = nullptr;
    HGLRC m_hglrc = nullptr;
};
