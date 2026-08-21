#include "TransformGizmo.hpp"

#include "../EditorContext.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "../EditorDrawList.hpp"

void TransformGizmo::draw(
    Scene* scene,
    Entity e,
    EditorContext& ctx
)
{
    auto* transform =
        scene->getComponent<Transform>(e);

    auto camera = ctx.editorCamera;

    auto* camComponent = scene->getComponent<CameraComponent>(camera);
    auto* camTransform = scene->getComponent<Transform>(camera);

    if (!camComponent || !camTransform || !transform)
        return;

    glm::vec3 pos =
        transform->position;

    float axisLength = 1.0f;

    ctx.drawList->arrow(
        pos,
        pos + glm::vec3(axisLength, 0, 0),
        camTransform->position,
        { 1, 0, 0, 1 },
        2.0
    );

    ctx.drawList->arrow(
        pos,
        pos + glm::vec3(0, axisLength, 0),
        camTransform->position,
        { 0, 1, 0, 1 },
        2.0
    );

    ctx.drawList->arrow(
        pos,
        pos + glm::vec3(0, 0, axisLength),
        camTransform->position,
        { 0, 0, 1, 1 },
        2.0
    );
}