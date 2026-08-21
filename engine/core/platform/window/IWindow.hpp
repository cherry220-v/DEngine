#pragma once

struct WindowProps {
    int width = 800;
    int height = 600;
    const char* title = "DEngine - Title";
};

enum CursorMode {
    Normal,
    Locked,
    Hidden,
};

class IWindow
{
public:
	virtual ~IWindow() = default;

    virtual int getWidth() = 0;
    virtual int getHeight() = 0;

	virtual void pollEvents() = 0;
    virtual void swapBuffers() = 0;
    virtual bool shouldClose() const = 0;

    virtual void* getNativeHandle() = 0;

    virtual void makeContextCurrent() = 0;

    virtual void setCursorMode(CursorMode mode) = 0;
    virtual CursorMode getCursorMode() = 0;

};