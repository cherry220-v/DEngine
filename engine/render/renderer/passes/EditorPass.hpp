#pragma once

#include <tinygizmo/tiny-gizmo.hpp>

#include <render/RenderContext.hpp>

#include "IRenderPass.hpp"
#include <render/renderer/IRenderDevice.hpp>
#include <render/renderer/RenderGraphStorage.hpp>

#include <editor/DrawList.hpp>

#include <vector>

class Shader;

class EditorPass : public IRenderPass
{
public:
    void init(IRenderPlatform* platform, IRenderDevice* device) override;

    void setup(RenderGraphStorage& graph) override;

    void execute(
        const RenderContext& ctx,
        RenderGraphStorage& graph
    ) override;

    void RenderDrawList(bool depth=false);
private:
    IRenderDevice* device = nullptr;
    IRenderPlatform* platform = nullptr;

    std::shared_ptr<Shader> gizmoShader;

    unsigned int selectedEntity = 0;
    int selectedEntityHandleId;

    unsigned int debugVBO = -1;
    unsigned int debugVAO = -1;
    unsigned int debugIBO = -1;

    DrawList m_drawList;
	tinygizmo::gizmo_context gizmo_ctx;
};
