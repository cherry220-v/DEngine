#include "SelectPass.hpp"
#include <math/CameraMath.h>
#include <ecs/Scene.hpp>

#include <core/platform/input/IInput.hpp>

#include <assets/AssetManager.hpp>
#include <math/Triangle.hpp>
#include <render/mesh/Mesh.hpp>

void SelectPass::init(IRenderPlatform* platform, IRenderDevice* device)
{
    this->platform = platform;
    this->device = device;

    auto& assetManager = AssetManager::get();

    selectionShader = assetManager.load<Shader>("876f5210-bd88-4ae0-a5cd-26a04c57ec50");

    selectionFBO = device->createFramebuffer();

    selectionMap = device->createDepthTexture(
        platform->getWidth(),
        platform->getHeight()
    );

    device->attachDepthTexture(
        selectionFBO,
        selectionMap
    );
}

void SelectPass::setup(RenderGraphStorage& graph)
{
    selectedEntityHandleId = graph.create("SelectPass.SelectedEntity");

    writes.clear();
    writes.push_back(selectedEntityHandleId);

    graph.publish(
        "SelectPass.SelectedEntity",
        selectedEntityHandleId
    );
    graph.set<unsigned int>(selectedEntityHandleId, std::make_shared<unsigned int>(0));
}

void SelectPass::execute(const RenderContext& context, RenderGraphStorage& graph)
{
    device->bindFramebuffer(selectionFBO);
    device->setViewport(platform->getWidth(), platform->getHeight());

    device->setDepthTestEnabled(true);
    device->clearColorBufferUInt(0);

    device->clearDepth();

    Scene* scene = context.scene;
    auto* camTransform = scene->getComponent<Transform>(context.activeCamera);
    auto* cam = scene->getComponent<CameraComponent>(context.activeCamera);

    if (!cam || !camTransform)
        return;

    selectionShader->use();

    for (auto e : scene->view<ModelRendererComponent, Transform>())
    {
        auto* renderer = scene->getComponent<ModelRendererComponent>(e);
        auto* transform = scene->getComponent<Transform>(e);

        if (!bool(renderer->model) || !bool(renderer->material)) return;

        float tMeshAABB = 0.0f;

        auto model = renderer->model.get();
    }
    double mouseX;
    double mouseY;
    context.input->getMousePosition(mouseX, mouseY);
    if (!context.input->getMouseButton(MouseButton::Left)) return;
    auto selectedEntity = pickObject(mouseX, mouseY);

    graph.set<unsigned int>(selectedEntityHandleId, std::make_shared<unsigned int>(selectedEntity));
}
;