#pragma once

class IRenderPlatform {
public:
    virtual ~IRenderPlatform() = default;

    virtual void makeCurrent() = 0;
    virtual void doneCurrent() = 0;

    virtual void swapBuffers() = 0;

    virtual void* getProcAddress(const char* name) = 0;

    virtual unsigned int getDefaultFBO() const = 0;

    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
};