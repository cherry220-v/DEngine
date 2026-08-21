#include "OpenGLRenderDevice.hpp"

void OpenGLRenderDevice::init(IRenderPlatform* platform)
{
    platform->makeCurrent();
    g_platform = platform;

    if (!gladLoadGLLoader((GLADloadproc)gladLoader)) {
        throw std::runtime_error("Failed to init GLAD");
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}
void* OpenGLRenderDevice::gladLoader(const char* name) {
    return g_platform->getProcAddress(name);
}
void OpenGLRenderDevice::setViewport(int w, int h)
{
    glViewport(0, 0, w, h);
}

void OpenGLRenderDevice::bindFramebuffer(unsigned int fbo)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void OpenGLRenderDevice::bindDefaultFramebuffer()
{
}

void OpenGLRenderDevice::bindTexture2D(unsigned int texture)
{
    bindTexture2D(texture, 1);
}

void OpenGLRenderDevice::bindTexture2D(unsigned int texture, unsigned int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void OpenGLRenderDevice::setBlendOneAEnabled(bool enabled)
{
    if (enabled) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
        glDisable(GL_BLEND);
}

void OpenGLRenderDevice::setBlendOneEnabled(bool enabled)
{
    if (enabled) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }
    else
        glDisable(GL_BLEND);
}

void OpenGLRenderDevice::setDepthTestEnabled(bool enabled)
{
    if (enabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

void OpenGLRenderDevice::setDepthMaskEnabled(bool enabled)
{
    if (enabled)
        glDepthMask(GL_TRUE);
    else
        glDepthMask(GL_FALSE);
}

void OpenGLRenderDevice::drawArrays(int mode, int first, int count)
{
    glDrawArrays(mode, first, count);
}

void OpenGLRenderDevice::clearDepth()
{
    glClear(GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderDevice::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderDevice::clearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

void OpenGLRenderDevice::clearColorBufferUInt(unsigned int value)
{
    glClearBufferuiv(
        GL_COLOR,
        0,
        &value
    );
}

unsigned int OpenGLRenderDevice::createFramebuffer()
{
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    return fbo;
}

unsigned int OpenGLRenderDevice::createDepthTexture(int w, int h)
{
    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT,
        w,
        h,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        nullptr
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    const float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    return tex;
}
void OpenGLRenderDevice::attachDepthTexture(unsigned int fbo, unsigned int tex)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        tex,
        0
    );

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

void OpenGLRenderDevice::setPolygonMode(PolygonMode mode)
{
    switch (mode)
    {
    case PolygonMode::Fill:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;

    case PolygonMode::Line:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    }
}

void OpenGLRenderDevice::setCullEnabled(bool enabled)
{
    if (enabled)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
}

unsigned int OpenGLRenderDevice::readPixels(float width, float height)
{
    unsigned int id = 0;

    glReadPixels(
        width,
        height,
        1,
        1,
        GL_RED_INTEGER,
        GL_UNSIGNED_INT,
        &id
    );

    return id;
}

unsigned int OpenGLRenderDevice::createVertexArray()
{
    unsigned int vao;

    glGenVertexArrays(1, &vao);

    return vao;
}

unsigned int OpenGLRenderDevice::createVertexBuffer(
    const void* data,
    size_t size)
{
    unsigned int vbo;

    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        size,
        data,
        GL_STATIC_DRAW
    );

    return vbo;
}

void OpenGLRenderDevice::bindVertexArray(
    unsigned int vao)
{
    glBindVertexArray(vao);
}

void OpenGLRenderDevice::bindVertexBuffer(
    unsigned int vbo)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

GLenum convertDataType(DataType type)
{
    switch (type)
    {
    case DataType::Float:
        return GL_FLOAT;

    case DataType::Int:
        return GL_INT;

    case DataType::UInt:
        return GL_UNSIGNED_INT;
    }

    return GL_FLOAT;
}

void OpenGLRenderDevice::setVertexAttrib(
    unsigned int index,
    int size,
    DataType type,
    bool normalized,
    size_t stride,
    size_t offset)
{
    glVertexAttribPointer(
        index,
        size,
        convertDataType(type),
        normalized ? GL_TRUE : GL_FALSE,
        static_cast<GLsizei>(stride),
        reinterpret_cast<void*>(offset)
    );
}

void OpenGLRenderDevice::enableVertexAttrib(
    unsigned int index)
{
    glEnableVertexAttribArray(index);
}