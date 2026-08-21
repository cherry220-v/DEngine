#include <glad/glad.h>
#include "OpenGLRenderer.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

#include <math/CameraMath.h>

//#define QTGL_CALL(x) \
//    do { \
//        x; \
//        GLenum err = glGetError(); \
//        if (err != GL_NO_ERROR) { \
//            qDebug() << "OpenGL Error: " << err \
//                      << " at " << __FILE__ << ":" << __LINE__ \
//                      << " in " << #x; \
//        } \
//} while (0)

IRenderPlatform* OpenGLRenderer::g_platform = nullptr;

void printMatrix(const std::string & name, const glm::mat4 & mat) {
    std::cout << name << ":" << std::endl;
    const float* p = glm::value_ptr(mat);
    for (int i = 0; i < 4; i++) {
        std::cout << "  [ ";
        for (int j = 0; j < 4; j++) {
            std::cout << mat[j][i] << " ";
        }
        std::cout << "]" << std::endl;
    }
}

OpenGLRenderer::OpenGLRenderer() : assetManager(AssetManager::get())
{
}

void* OpenGLRenderer::gladLoader(const char* name) {
    return g_platform->getProcAddress(name);
}

void OpenGLRenderer::init() {
    if (!platform) return;
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

    //glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
}

void OpenGLRenderer::renderFrame(const RenderContext& ctx) {
    if (!platform) return;

    if (
        shadowShader == nullptr || selectionShader == nullptr ||
        shadowMat == nullptr || selectionMat == nullptr
        ) {
        initShaders(); return;
    }

    modeSwitch(SELECTION, ctx);
    modeSwitch(SHADOW, ctx);

    glBindFramebuffer(GL_FRAMEBUFFER, platform->getDefaultFBO());
    glViewport(0, 0, platform->getWidth(), platform->getHeight());

    beginFrame();

    modeSwitch(SCENE, ctx);

    endFrame();
}

void OpenGLRenderer::modeSwitch(RenderMode mode, const RenderContext& context)
{
}

void OpenGLRenderer::beginFrame()
{
    glDepthFunc(GL_LESS);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::endFrame()
{

}

void OpenGLRenderer::initShaders() {
    shadowShader = assetManager.gett<Shader>("b3d97c0b-5a51-44ab-9437-df461b3e8615");
    shadowMat = assetManager.gett<Material>("50e6fbf1-4eee-4a81-b08f-14aabdbf16d5");

    selectionShader = assetManager.gett<Shader>("876f5210-bd88-4ae0-a5cd-26a04c57ec50");
    selectionMat = assetManager.gett<Material>("538c1570-b528-439a-8069-67cfe2360384");
}
