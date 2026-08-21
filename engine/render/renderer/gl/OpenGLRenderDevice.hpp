#pragma once
#include <glad/glad.h>
#include "../IRenderDevice.hpp"
#include <stdexcept>

class OpenGLRenderDevice : public IRenderDevice
{
    inline static IRenderPlatform* g_platform = nullptr;
public:
    void init(IRenderPlatform* platform) override;
    static void* gladLoader(const char* name);
    void setViewport(int w, int h) override;

    void bindFramebuffer(unsigned int fbo) override;

    void bindDefaultFramebuffer() override;

    void bindTexture2D(unsigned int texture) override;
    void bindTexture2D(unsigned int texture, unsigned int unit) override;

    void setBlendOneAEnabled(bool enabled) override;

    void setBlendOneEnabled(bool enabled) override;

    void setDepthTestEnabled(bool enabled) override;
    void setDepthMaskEnabled(bool enabled) override;

    void drawArrays(int mode, int first, int count) override;

    void clearDepth() override;

    void clear() override;

    void clearColor(float r, float g, float b, float a) override;

    void clearColorBufferUInt(unsigned int value);

    unsigned int createFramebuffer() override;

    unsigned int createDepthTexture(int w, int h) override;
    void attachDepthTexture(unsigned int fbo, unsigned int tex) override;

    unsigned int createVertexArray() override;

    unsigned int createVertexBuffer(
        const void* data,
        size_t size
    ) override;

    void bindVertexArray(
        unsigned int vao
    ) override;

    void bindVertexBuffer(
        unsigned int vbo
    ) override;

    void setVertexAttrib(
        unsigned int index,
        int size,
        DataType type,
        bool normalized,
        size_t stride,
        size_t offset
    ) override;

    void enableVertexAttrib(
        unsigned int index
    ) override;

    void setPolygonMode(PolygonMode mode);

    void setCullEnabled(bool enabled);

    unsigned int readPixels(float width, float height) override;
};