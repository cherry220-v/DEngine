#include "OverlayPass.hpp"

#include <assets/AssetManager.hpp>

#include <overlay/OverlayMath.hpp>
#include <overlay/GizmoRenderer.hpp>
#include <overlay/GizmoContext.hpp>

#include <ecs/components/Transform.hpp>
#include <math/CameraMath.h>

#include <core/platform/input/IInput.hpp>
#include <math/PickObject.hpp>

using namespace OverlayMath;

static auto gizmoCtx = GizmoContext();

void OverlayPass::init(IRenderPlatform* platform, IRenderDevice* device)
{
	this->platform = platform;
	this->device = device;

	auto& assetManager = AssetManager::get();
	gizmoShader = assetManager.load<Shader>("9d4b2d0c-0d77-4cf5-bd98-ff9a2332e2ff");
}

void OverlayPass::setup(RenderGraphStorage& graph)
{
}

void OverlayPass::execute(const RenderContext& ctx, RenderGraphStorage& graph)
{
	device->bindFramebuffer(platform->getDefaultFBO());
    m_drawList.Clear();
    auto scene = ctx.scene;
	auto input = ctx.input;

    auto* camTransform = scene->getComponent<Transform>(ctx.activeCamera);
    auto* cam = scene->getComponent<CameraComponent>(ctx.activeCamera);
    if (!cam || !camTransform) return;

    glm::mat4 viewMatrix = glm::inverse(camTransform->worldMatrix);
    glm::mat4 projectionMatrix = getProjectionMatrix(*cam);

    std::cout << "view matrix " << viewMatrix << std::endl;
    std::cout << "proj matrix " << projectionMatrix << std::endl;
	std::cout << "cam pos " << camTransform->position << std::endl;

    if (input->getMouseButton(MouseButton::Left))
    {
        double mouseX, mouseY;
        input->getMousePosition(mouseX, mouseY);

        PickingResult result = doCPUPicking(scene, mouseX, mouseY, platform->getWidth(), platform->getHeight(), ctx.activeCamera);
        std::cout << result.entityID << std::endl;

        if (result.entityID != 0) {
            scene->selectEntity(result.entityID);
        }
        else {
            scene->clearSelection();
        }
    }

    for (auto entity : scene->getSelectedEntities())
    {
        auto* transform = scene->getComponent<Transform>(entity);
        if (!transform) continue;

        glm::vec3 pos = transform->position;


        GizmoRender::DrawTranslateGizmo(
            m_drawList,
            transform->getMatrix()[3],
            viewMatrix,
            projectionMatrix,
            {
                (float)platform->getWidth(),
                (float)platform->getHeight()
            });
        std::cout << "TRANSFORM " << transform->getMatrix() << std::endl;
    }

    auto shader = AssetManager::get().load<Shader>("ea7b519b-5f17-49b5-b8e6-51e340b37f37");
    shader->use();
    for (const DrawCmd& cmd : m_drawList.cmdBuffer)
    {
        // -------------------------
        // Depth state
        // -------------------------

        if (cmd.depthTest)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);

        glDepthMask(cmd.writeDepth ? GL_TRUE : GL_FALSE);

        // -------------------------
        // Blend
        // -------------------------

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // -------------------------
        // Scissor
        // -------------------------

        glEnable(GL_SCISSOR_TEST);

        int x = (int)cmd.clipRect.Min.x;
        int y = platform->getHeight() - (int)cmd.clipRect.Max.y;

        int w = (int)(cmd.clipRect.Max.x - cmd.clipRect.Min.x);
        int h = (int)(cmd.clipRect.Max.y - cmd.clipRect.Min.y);

        glScissor(x, y, w, h);

        // -------------------------
        // Matrices
        // -------------------------
        int width = platform->getWidth();
        int height = platform->getHeight();

        glm::mat4 orthoMatrix = glm::ortho(
            0.0f,
            (float)width,
            (float)height,
            0.0f,
            -1.0f,
            1.0f
        );

		std::cout << "ortho matrix " << orthoMatrix << std::endl;
		std::cout << "viewproj" << projectionMatrix * viewMatrix << std::endl;

        switch (cmd.space)
        {
        case DrawSpace::Screen:
        {
            shader->setMat4("uMVP", orthoMatrix);
            break;
        }

        case DrawSpace::World:
        {
            shader->setMat4("uMVP", projectionMatrix * viewMatrix);
            break;
        }

        case DrawSpace::WorldOverlay:
        {
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
            break;
        }
        }

        // -------------------------
        // Texture
        // -------------------------

        glBindTexture(GL_TEXTURE_2D, cmd.textureId);

        // -------------------------
        // Draw
        // -------------------------
        glDrawElementsBaseVertex(
            GL_TRIANGLES,
            cmd.elemCount,
            GL_UNSIGNED_INT, // DrawIdx = uint32_t
            (void*)(cmd.idxOffset * sizeof(DrawIdx)),
            cmd.vtxOffset
        );
    }
}
