#include "EditorPass.hpp"

#include <math/CameraMath.h>

#include <editor/EditorContext.hpp>
#include <editor/gizmo/ImGuizmo.h>

#include <render/shader/Shader.hpp>
#include <assets/AssetManager.hpp>

#include <math/Vertex.h>
#include <editor/DrawList.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tinygizmo/tiny-gizmo.hpp>
#include <editor/gizmo/ImGuizmo.h>
#include <math/PickObject.hpp>

void checkGLError(const char* place)
{
    GLenum err;

    while ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cout
            << "[OpenGL] "
            << place
            << ": ";

        switch (err)
        {
        case GL_INVALID_ENUM:
            std::cout << "GL_INVALID_ENUM";
            break;

        case GL_INVALID_VALUE:
            std::cout << "GL_INVALID_VALUE";
            break;

        case GL_INVALID_OPERATION:
            std::cout << "GL_INVALID_OPERATION";
            break;

        case GL_INVALID_FRAMEBUFFER_OPERATION:
            std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;

        case GL_OUT_OF_MEMORY:
            std::cout << "GL_OUT_OF_MEMORY";
            break;

        default:
            std::cout << err;
            break;
        }

        std::cout << std::endl;
    }
}

void EditorPass::init(IRenderPlatform* platform, IRenderDevice* device)
{
	this->platform = platform;
	this->device = device;

    auto& assetManager = AssetManager::get();
    gizmoShader = assetManager.load<Shader>("9d4b2d0c-0d77-4cf5-bd98-ff9a2332e2ff");
}

void EditorPass::setup(RenderGraphStorage& graph) {
    debugVAO = device->createVertexArray();
    debugVBO = device->createVertexBuffer(nullptr, 0);
    glGenBuffers(1, &debugIBO);
    device->bindVertexArray(debugVAO);
    device->bindVertexBuffer(debugVBO);
    device->setVertexAttrib(0, 3, DataType::Float, false, sizeof(GizmoVertex), offsetof(GizmoVertex, position));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, debugIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    device->enableVertexAttrib(0);
    device->setVertexAttrib(1, 4, DataType::Float, false, sizeof(GizmoVertex), offsetof(GizmoVertex, color));
    device->enableVertexAttrib(1);
}

void EditorPass::execute(const RenderContext& ctx, RenderGraphStorage& graph)
{
//    device->bindFramebuffer(platform->getDefaultFBO());
//    m_drawList.vtxBuffer.clear();
//    m_drawList.idxBuffer.clear();
//    m_drawList.cmdBuffer.clear();
//    m_drawList._Path.clear();
//    m_drawList._ClipRectStack.clear();
//    m_drawList._TextureStack.clear();
//    m_drawList._VtxWritePtr = nullptr;
//    m_drawList._IdxWritePtr = nullptr;
//    m_drawList._VtxCurrentIdx = 0;
//    
//    m_drawList.PushClipRect(glm::vec2(0.0f, 0.0f), glm::vec2((float)platform->getWidth(), (float)platform->getHeight()), false);
//    m_drawList.UpdateTexture();
//
//    auto scene = ctx.scene;
//	auto input = ctx.input;
//    
//    auto* camTransform = scene->getComponent<Transform>(ctx.activeCamera);
//    auto* cam = scene->getComponent<CameraComponent>(ctx.activeCamera);
//    if (!cam || !camTransform) return;
//    
//    glm::mat4 viewMatrix = glm::inverse(camTransform->worldMatrix);
//    glm::mat4 projectionMatrix = getProjectionMatrix(*cam);
//    
//    if (input->getMouseButton(MouseButton::Left))
//    {
//        double mouseX, mouseY;
//        input->getMousePosition(mouseX, mouseY);
//
//        PickingResult result = doCPUPicking(scene, mouseX, mouseY, platform->getWidth(), platform->getHeight(), ctx.activeCamera);
//        std::cout << result.entityID << std::endl;
//
//        if (result.entityID != 0) {
//            scene->selectEntity(result.entityID);
//        }
//        else {
//            scene->clearSelection();
//        }
//    }
//
//    GUIZMO_NAMESPACE::UpdateContext(ctx, m_drawList, platform, device);
//
//    Guizmo::SetOrthographic(false);
//
//    glm::mat4 gridModelMatrix = glm::mat4(1.0f);
//
//    glm::vec4 p =
//        projectionMatrix *
//        viewMatrix *
//        glm::vec4(0, 0, 0, 1);
//
//    Guizmo::DrawGrid(
//        glm::value_ptr(viewMatrix),        // view
//        glm::value_ptr(projectionMatrix),        // projection
//        glm::value_ptr(gridModelMatrix),   // matrix (положение сетки в мире)
//        1900.0f                             // gridSize (размер сетки, например 100 метров)
//    );
//    RenderDrawList();
//}
//
//void EditorPass::RenderDrawList(bool depth)
//{
//    std::vector<GizmoVertex> gv;
//    gv.reserve(m_drawList.vtxBuffer.size());
//    for (const DrawVert& dv : m_drawList.vtxBuffer)
//    {
//        GizmoVertex v;
//        v.position = glm::vec3(dv.pos[0], dv.pos[1], 0.0f);
//        uint32_t c = dv.col;
//        float r = float((c) & 0xFF) / 255.0f;
//        float g = float((c >> 8) & 0xFF) / 255.0f;
//        float b = float((c >> 16) & 0xFF) / 255.0f;
//        float a = float((c >> 24) & 0xFF) / 255.0f;
//        v.color = glm::vec4(r, g, b, a); gv.push_back(v);
//    }
//    if (debugIBO == 0) {
//        glGenBuffers(1, &debugIBO);
//
//        device->bindVertexArray(debugVAO);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, debugIBO);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW); glBindVertexArray(0);
//    }
//
//    device->bindVertexArray(debugVAO); glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
//    if (!gv.empty()) glBufferData(GL_ARRAY_BUFFER, gv.size() * sizeof(GizmoVertex), gv.data(), GL_DYNAMIC_DRAW); else glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, debugIBO); if (!m_drawList.idxBuffer.empty()) glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_drawList.idxBuffer.size() * sizeof(DrawIdx), m_drawList.idxBuffer.data(), GL_DYNAMIC_DRAW); else glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW); gizmoShader->use(); glm::mat4 screenOrtho = glm::ortho(0.0f, (float)platform->getWidth(), (float)platform->getHeight(), 0.0f, -1.0f, 1.0f); gizmoShader->setMat4("uProjection", screenOrtho); gizmoShader->setMat4("uView", glm::mat4(1.0f));
//    device->setDepthTestEnabled(depth);
//    glDepthMask(!depth);
//    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glBlendEquation(GL_FUNC_ADD);
//    //std::cout << "DBG: debugIBO=" << debugIBO << " vtx=" << m_drawList.vtxBuffer.size() << " idx=" << m_drawList.idxBuffer.size() << " cmd=" << m_drawList.cmdBuffer.size() << std::endl;
//    size_t sumCmds = 0;
//    for (auto& c : m_drawList.cmdBuffer) 
//    {
//        //std::cout << " cmd elems=" << c.elemCount << " tex=" << c.textureId << std::endl; sumCmds += c.elemCount; 
//    } //std::cout << "DBG: sumCmdElems=" << sumCmds << std::endl;
//    size_t indexOffset = 0; for (auto& cmd : m_drawList.cmdBuffer) { if (cmd.elemCount == 0) continue; if (indexOffset + cmd.elemCount > m_drawList.idxBuffer.size()) {
//        std::cout << "DrawList: index overflow, skip remaining cmds\n"; break; } if (cmd.textureId) { device->bindTexture2D(cmd.textureId); } else { device->bindTexture2D(Texture::getWhiteTexture().getID()); 
//        } glEnable(GL_SCISSOR_TEST); glScissor(static_cast<int>(cmd.clipRect.Min.x), static_cast<int>(platform->getHeight() - cmd.clipRect.Max.y), static_cast<int>(cmd.clipRect.Max.x - cmd.clipRect.Min.x), static_cast<int>(cmd.clipRect.Max.y - cmd.clipRect.Min.y)); const void* offsetPtr = reinterpret_cast<const void*>(indexOffset * sizeof(DrawIdx)); glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cmd.elemCount), GL_UNSIGNED_SHORT, offsetPtr); indexOffset += cmd.elemCount; } device->setDepthTestEnabled(true); glDisable(GL_SCISSOR_TEST);
}