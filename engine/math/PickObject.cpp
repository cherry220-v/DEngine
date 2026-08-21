#include "PickObject.hpp"

Ray calculateRayFromMouse(double mouseX, double mouseY, int screenWidth, int screenHeight, const glm::mat4& viewMatrix, const glm::mat4& projMatrix)
{
    if (screenWidth <= 0 || screenHeight <= 0) {
        return Ray{ glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f) };
    }

    float ndcX = (2.0f * (float)mouseX) / (float)screenWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * (float)mouseY) / (float)screenHeight;

    glm::vec4 clipCoords(ndcX, ndcY, -1.0f, 1.0f);

    glm::mat4 invProj = glm::inverse(projMatrix);
    glm::vec4 eyeCoords = invProj * clipCoords;
    eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);

    glm::mat4 invView = glm::inverse(viewMatrix);
    glm::vec3 worldDir = glm::vec3(invView * eyeCoords);
    worldDir = glm::normalize(worldDir);

    glm::vec3 worldOrigin = glm::vec3(invView[3][0], invView[3][1], invView[3][2]);

    return Ray{ worldOrigin, worldDir };
}

PickingResult doCPUPicking(Scene* scene, double mouseX, double mouseY, int screenWidth, int screenHeight, unsigned int activeCamera)
{
    PickingResult closestHit;

    auto* camTransform = scene->getComponent<Transform>(activeCamera);
    auto* cam = scene->getComponent<CameraComponent>(activeCamera);

    if (!cam || !camTransform) {
        return closestHit;
    }

    glm::mat4 viewMatrix = getViewMatrix(*camTransform);
    glm::mat4 projMatrix = getProjectionMatrix(*cam);

    Ray worldRay = calculateRayFromMouse(mouseX, mouseY, screenWidth, screenHeight, viewMatrix, projMatrix);

    for (auto e : scene->view<ModelRendererComponent, Transform>())
    {
        auto* renderer = scene->getComponent<ModelRendererComponent>(e);
        if (!renderer || !renderer->model) continue;

        auto model = renderer->model.get();
        glm::mat4 worldMatrix = scene->getWorldMatrix(e);
        glm::mat4 invWorldMatrix = glm::inverse(worldMatrix);

        Ray localRay;
        localRay.origin = glm::vec3(invWorldMatrix * glm::vec4(worldRay.origin, 1.0f));

        glm::vec3 rawLocalDir = glm::vec3(invWorldMatrix * glm::vec4(worldRay.direction, 0.0f));
        float lengthRawDir = glm::length(rawLocalDir);

        if (lengthRawDir < 0.00001f) {
            continue;
        }

        localRay.direction = rawLocalDir / lengthRawDir;


        float tModelAABB = 0.0f;
        bool hitModel = hitAABB(localRay, model->localAABB, tModelAABB);

        if (!hitModel) continue;

        float worldDistanceModel = tModelAABB / lengthRawDir;
        if (worldDistanceModel > closestHit.distance) {
            continue;
        }

        int meshIndex = 0;

        for (auto meshPtr : model->meshes) {
            if (!meshPtr) { meshIndex++; continue; }

            auto& mesh = *meshPtr;
            float tMeshAABB = 0.0f;
            bool hitMesh = hitAABB(localRay, mesh.localAABB, tMeshAABB);

            if (hitMesh) {
                size_t triCount = mesh.getTriangleCount();

                int hitTrianglesCount = 0;

                for (size_t i = 0; i < triCount; ++i) {
                    Triangle tri = mesh.getTriangle(i);

                    if (i == 0 && meshIndex == 0) {
                    }

                    float tTri = 0.0f;
                    float u = 0.0f, v = 0.0f;

                    if (hitTriangle(localRay, tri, tTri, u, v)) {
                        hitTrianglesCount++;

                        float worldDistance = tTri / lengthRawDir;

                        if (worldDistance < closestHit.distance)
                        {
                            closestHit.entityID = e;
                            closestHit.distance = worldDistance;

                            glm::vec3 localHitPoint =
                                localRay.origin + localRay.direction * tTri;

                            closestHit.worldHitPoint =
                                glm::vec3(
                                    worldMatrix * glm::vec4(localHitPoint, 1.0f)
                                );

                        }
                    }
                }
                if (hitTrianglesCount > 0) {
                }
            }
        }
        meshIndex++;
    }

if (closestHit.entityID != 0) {
}

return closestHit;
}