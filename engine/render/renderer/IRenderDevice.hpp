#pragma once

#include <core/platform/render/IRenderPlatform.hpp>

enum class PolygonMode
{
    Fill,
    Line
};

enum class DataType
{
    Float,
    Int,
    UInt
};

enum DrawMode {
    Lines = 0x0001,
    Triangles = 0x0004
};

class IRenderDevice
{
public:
    virtual void init(IRenderPlatform* platform) = 0;

    virtual void setViewport(int w, int h) = 0;
    virtual void bindFramebuffer(unsigned int fbo) = 0;
    virtual void bindDefaultFramebuffer() = 0;
    virtual void bindTexture2D(unsigned int texture) = 0;
    virtual void bindTexture2D(unsigned int texture, unsigned int unit) = 0;

    virtual void setDepthTestEnabled(bool enabled) = 0;
    virtual void setDepthMaskEnabled(bool enabled) = 0;
    virtual void clearDepth() = 0;
    virtual void clear() = 0;
    virtual void clearColor(float r, float g, float b, float a) = 0;
    virtual void clearColorBufferUInt(unsigned int value) = 0;
    virtual void setBlendOneAEnabled(bool enabled) = 0;

    virtual void setBlendOneEnabled(bool enabled) = 0;
    virtual void setPolygonMode(PolygonMode mode) = 0;

    virtual void setCullEnabled(bool enabled) = 0;

    virtual void drawArrays(int mode, int first, int count) = 0;

    virtual unsigned int readPixels(float width, float height) = 0;

    virtual unsigned int createFramebuffer() = 0;
    virtual unsigned int createDepthTexture(int w, int h) = 0;
    virtual unsigned int createVertexArray() = 0;

    virtual unsigned int createVertexBuffer(
        const void* data,
        size_t size
    ) = 0;

    virtual void bindVertexArray(
        unsigned int vao
    ) = 0;

    virtual void bindVertexBuffer(
        unsigned int vbo
    ) = 0;

    virtual void setVertexAttrib(
        unsigned int index,
        int size,
        DataType type,
        bool normalized,
        size_t stride,
        size_t offset
    ) = 0;

    virtual void enableVertexAttrib(
        unsigned int index
    ) = 0;

    virtual void attachDepthTexture(unsigned int fbo, unsigned int tex) = 0;
};