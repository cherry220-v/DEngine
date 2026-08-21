#pragma once

#include <math/CameraMath.h>
#include <ecs/Scene.hpp>

#include <core/platform/input/IInput.hpp>

#include <assets/AssetManager.hpp>
#include <math/Triangle.hpp>
#include <render/mesh/Mesh.hpp>

#include <ecs/components/ModelRendererComponent.hpp>

#include <glm/glm.hpp>
#include <limits>

#include <core/prints.h>

struct PickingResult {
    unsigned int entityID = 0;
    glm::vec3 worldHitPoint{ 0.0f };
    float distance = std::numeric_limits<float>::max();
};

Ray calculateRayFromMouse(double mouseX, double mouseY, int screenWidth, int screenHeight,
    const glm::mat4& viewMatrix, const glm::mat4& projMatrix);


PickingResult doCPUPicking(Scene* scene, double mouseX, double mouseY, int screenWidth, int screenHeight, unsigned int activeCamera);