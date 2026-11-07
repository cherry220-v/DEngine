#pragma once

#include <render/RenderContext.hpp>
#include "IRenderPass.hpp"
#include <render/renderer/IRenderDevice.hpp>
#include <render/renderer/RenderGraphStorage.hpp>

#include <editor/DrawList.hpp>

#include <memory>
#include <vector>

class Shader;

// Editor overlay pass. Renders on top of the scene (default framebuffer):
//
//   * the world grid  - Guizmo::DrawGrid from the engine ImGuizmo port
//   * the translation gizmo - Guizmo::Manipulate(TRANSLATE) for the currently
//     selected entity, shown after clicking it in the viewport
//   * click-to-select via doCPUPicking (edge-triggered, suppressed while the
//     mouse interacts with the gizmo)
//
// Everything is recorded as 2D primitives into a DrawList (pixel space,
// top-left origin) and pushed to the GPU in RenderDrawList().
class OverlayPass : public IRenderPass
{
public:
    void init(IRenderPlatform* platform, IRenderDevice* device) override;

    void setup(RenderGraphStorage& graph) override;

    void execute(
        const RenderContext& ctx,
        RenderGraphStorage& graph
    ) override;

    // Uploads the DrawList buffers and renders every DrawCmd.
    // depth is kept for API compatibility and currently unused.
    void RenderDrawList(bool depth = false);

private:
    IRenderDevice* device = nullptr;
    IRenderPlatform* platform = nullptr;

    std::shared_ptr<Shader> overlayShader;

    // GL objects for DrawList rendering (layout matches engine DrawVert)
    unsigned int drawVAO = 0;
    unsigned int drawVBO = 0;
    unsigned int drawIBO = 0;

    // 1x1 white texture bound for commands without a texture, so untextured
    // primitives keep their vertex colors (tex * vtxColor == vtxColor).
    unsigned int whiteTexture = 0;

    // half-extent of the world grid in units (grid spans -size..+size)
    float gridSize = 10.0f;

    DrawList m_drawList;
};
