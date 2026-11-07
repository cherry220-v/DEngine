#include "OverlayPass.hpp"

#include <glad/glad.h>
#include <cstddef>
#include <cstdint>

#include <assets/AssetManager.hpp>
#include <render/shader/Shader.hpp>

// ImGuizmo port (glm / column-major, OpenGL conventions)
#include <editor/gizmo/ImGuizmo.h>

#include <ecs/Scene.hpp>
#include <ecs/components/Transform.hpp>
#include <ecs/components/CameraComponent.hpp>
#include <ecs/entity/Entity.hpp>
#include <math/CameraMath.h>
#include <math/PickObject.hpp>
#include <core/platform/input/IInput.hpp>

#include <glm/gtc/type_ptr.hpp>

void OverlayPass::init(IRenderPlatform* platform, IRenderDevice* device)
{
    this->platform = platform;
    this->device = device;

    overlayShader = AssetManager::get().load<Shader>(
        "ea7b519b-5f17-49b5-b8e6-51e340b37f37" // Custom/Overlay
    );

    static_assert(sizeof(DrawVert) == 24,
        "DrawVert layout changed - update the overlay vertex attribute setup");

    // -------------------------
    // DrawList GL objects
    // -------------------------

    glGenVertexArrays(1, &drawVAO);
    glGenBuffers(1, &drawVBO);
    glGenBuffers(1, &drawIBO);

    glBindVertexArray(drawVAO);
    glBindBuffer(GL_ARRAY_BUFFER, drawVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawIBO);

    auto bindAttrib = [](GLuint index, GLint size, GLenum type, GLboolean normalized, size_t offset)
    {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(
            index,
            size,
            type,
            normalized,
            (GLsizei)sizeof(DrawVert),
            (const void*)offset
        );
    };

    bindAttrib(0, 2, GL_FLOAT, GL_FALSE, offsetof(DrawVert, pos));   // inPos
    bindAttrib(1, 2, GL_FLOAT, GL_FALSE, offsetof(DrawVert, uv));    // inUV
    bindAttrib(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, offsetof(DrawVert, col)); // inColor
    bindAttrib(3, 1, GL_FLOAT, GL_FALSE, offsetof(DrawVert, depth)); // inDepth

    glBindVertexArray(0);

    // -------------------------
    // 1x1 white fallback texture
    // -------------------------

    glGenTextures(1, &whiteTexture);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);

    const unsigned char white[4] = { 255, 255, 255, 255 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void OverlayPass::setup(RenderGraphStorage& graph)
{
    (void)graph;
}

void OverlayPass::execute(const RenderContext& ctx, RenderGraphStorage& graph)
{
    (void)graph;

    if (!ctx.scene || !ctx.input || !platform || !overlayShader)
    {
        return;
    }

    auto scene = ctx.scene;
    auto input = ctx.input;

    auto* camTransform = scene->getComponent<Transform>(ctx.activeCamera);
    auto* cam = scene->getComponent<CameraComponent>(ctx.activeCamera);
    if (!cam || !camTransform)
    {
        return;
    }

    device->bindFramebuffer(platform->getDefaultFBO());

    const float width = (float)platform->getWidth();
    const float height = (float)platform->getHeight();

    m_drawList.Clear();

    // The default DrawList clip rect is an empty rect: push the full viewport
    // first, otherwise the grid (which pushes no clip rect of its own) would
    // be scissored away completely.
    m_drawList.PushClipRect(
        glm::vec2(0.0f, 0.0f),
        glm::vec2(width, height),
        false
    );

    glm::mat4 viewMatrix = getViewMatrix(*camTransform);
    glm::mat4 projectionMatrix = getProjectionMatrix(*cam);

    // ------------------------------------------------------------------
    // 1) Update the gizmo port with this frame's camera/input/viewport
    // ------------------------------------------------------------------

    Guizmo::UpdateContext(ctx, m_drawList, platform, device);

    // ------------------------------------------------------------------
    // 2) World grid (решётка) - dedicated ImGuizmo port function
    // ------------------------------------------------------------------

    glm::mat4 gridMatrix = glm::mat4(1.0f);

    //Guizmo::DrawGrid(
    //    glm::value_ptr(viewMatrix),
    //    glm::value_ptr(projectionMatrix),
    //    glm::value_ptr(gridMatrix),
    //    gridSize
    //);

    // ------------------------------------------------------------------
    // 3) Translation gizmo on the selected entity
    //    Manipulate both draws the gizmo and, while it is dragged, writes
    //    the transformed world matrix back into worldMatrix.
    // ------------------------------------------------------------------

    bool gizmoActive = false;

    auto selected = scene->getSelectedEntities();
    if (!selected.empty())
    {
        Entity entity = selected.front();

        auto* transform = scene->getComponent<Transform>(entity);
        if (transform)
        {
            glm::mat4 worldMatrix = scene->getWorldMatrix(entity);
            glm::mat4 deltaMatrix(1.0f);

			std::cout << "Selected entity: " << entity << std::endl;

            bool manipulated = Guizmo::Manipulate(
                glm::value_ptr(viewMatrix),
                glm::value_ptr(projectionMatrix),
                Guizmo::TRANSLATE,
                Guizmo::WORLD,
                glm::value_ptr(worldMatrix),
                glm::value_ptr(deltaMatrix)
            );

            gizmoActive = true;

            if (manipulated)
            {
                glm::vec3 worldPos(worldMatrix[3]);

                // convert world position back into the entity's local space
                Entity parent = scene->getSceneGraph().getParent(entity);
                if (parent != NullEntity)
                {
                    glm::mat4 parentWorldInv =
                        glm::inverse(scene->getWorldMatrix(parent));

                    worldPos = glm::vec3(
                        parentWorldInv * glm::vec4(worldPos, 1.0f)
                    );
                }

                transform->position = worldPos;
            }
        }
    }

    // ------------------------------------------------------------------
    // 4) Click selection - edge triggered (press frame only) and skipped
    //    while the mouse is over / dragging the gizmo, so picking never
    //    fights with gizmo interaction.
    //    When no gizmo exists this frame (nothing selected / no Transform)
    //    the check is bypassed: the gizmo context hover state is stale then
    //    and must not swallow the click.
    // ------------------------------------------------------------------

    bool clickAllowed = !gizmoActive || (!Guizmo::IsUsing() && !Guizmo::IsOver());
	std::cout << "Click allowed: " << clickAllowed << std::endl;

    if (input->getMouseButton(MouseButton::Left) && clickAllowed)
    {
        double mouseX, mouseY;
        input->getMousePosition(mouseX, mouseY);

        PickingResult result = doCPUPicking(
            scene,
            mouseX, mouseY,
            platform->getWidth(),
            platform->getHeight(),
            ctx.activeCamera
        );
		std::cout << "Picked entity: " << result.entityID << std::endl;

        if (result.entityID != 0)
        {
            scene->selectEntity(result.entityID);
        }
        else
        {
            scene->clearSelection();
        }
    }

    m_drawList.PopClipRect();

    // ------------------------------------------------------------------
    // 5) Render the recorded DrawList on top of the scene
    // ------------------------------------------------------------------

    RenderDrawList();
}

void OverlayPass::RenderDrawList(bool depth)
{
    (void)depth;

    if (!platform || !overlayShader || m_drawList.cmdBuffer.empty())
    {
        return;
    }

    const int width = platform->getWidth();
    const int height = platform->getHeight();

    overlayShader->use();
    overlayShader->setVec2(
        "viewportSize",
        glm::vec2((float)width, (float)height)
    );

    // -------------------------
    // Upload buffers (streaming)
    // -------------------------

    glBindVertexArray(drawVAO);

    if (!m_drawList.vtxBuffer.empty())
    {
        glBindBuffer(GL_ARRAY_BUFFER, drawVBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            (GLsizeiptr)(m_drawList.vtxBuffer.size() * sizeof(DrawVert)),
            m_drawList.vtxBuffer.data(),
            GL_STREAM_DRAW
        );
    }

    if (!m_drawList.idxBuffer.empty())
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawIBO);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            (GLsizeiptr)(m_drawList.idxBuffer.size() * sizeof(DrawIdx)),
            m_drawList.idxBuffer.data(),
            GL_STREAM_DRAW
        );
    }

    // -------------------------
    // Global overlay state
    // -------------------------

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_SCISSOR_TEST);

    for (const DrawCmd& cmd : m_drawList.cmdBuffer)
    {
        if (cmd.elemCount == 0)
        {
            continue;
        }

        // Depth state
        if (cmd.depthTest)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }

        glDepthMask(cmd.writeDepth ? GL_TRUE : GL_FALSE);

        // Scissor - cmd clip rect is in top-left origin pixels,
        // GL wants bottom-left origin.
        int x = (int)cmd.clipRect.Min.x;
        int y = height - (int)cmd.clipRect.Max.y;
        int w = (int)(cmd.clipRect.Max.x - cmd.clipRect.Min.x);
        int h = (int)(cmd.clipRect.Max.y - cmd.clipRect.Min.y);

        if (w < 0) w = 0;
        if (h < 0) h = 0;

        glScissor(x, y, w, h);

        // Texture - 0 means "untextured": bind the white fallback so the
        // fragment multiply (texture * vertex color) keeps the vertex color.
        GLuint texture = (cmd.textureId != 0) ? (GLuint)cmd.textureId : whiteTexture;
        glBindTexture(GL_TEXTURE_2D, texture);

        // Draw - all primitives are triangles; the vertex shader maps the
        // pixel-space positions to NDC through the viewportSize uniform.
        glDrawElementsBaseVertex(
            GL_TRIANGLES,
            (GLsizei)cmd.elemCount,
            GL_UNSIGNED_INT,
            (const void*)(uintptr_t)(cmd.idxOffset * sizeof(DrawIdx)),
            (GLint)cmd.vtxOffset
        );
    }

    // -------------------------
    // Restore state for later passes / next frame
    // -------------------------

    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}
