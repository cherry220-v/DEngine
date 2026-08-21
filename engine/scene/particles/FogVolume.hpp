#pragma once

#include <glm/vec3.hpp>
#include <memory>

#include <math/TransformData.hpp>
#include <render/texture/Material.hpp>

struct FogVolume
{
    TransformData transform;

    glm::vec3 size{ 10.0f, 5.0f, 10.0f };
    
    float density = 0.03f;

    float edgeSoftness = 1.0f;

    float noiseScale = 1.0f;
    float noiseStrength = 0.2f;
    float animationSpeed = 0.1f;

    std::shared_ptr<Material> material;

    bool active = true;

    bool volumetric = true;
};