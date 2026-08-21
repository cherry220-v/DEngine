#pragma once

#include "Skybox.hpp"
#include "particles/EffectManager.hpp"

#include <glm/vec3.hpp>
#include <memory>

struct SceneContext
{
    std::shared_ptr<Skybox> skybox;
    std::shared_ptr<EffectManager> effectManager;

    glm::vec3 ambientColor = { 0.1f, 0.1f, 0.1f };
};