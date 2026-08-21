#include "EffectManager.hpp"
#include <math/Random.h>

#include <render/shader/Shader.hpp>
#include <render/texture/Material.hpp>

void EffectManager::update(float dt)
{
}

void EffectManager::updateFog(float dt) {}

void EffectManager::addFogVolume(const FogVolume& v)
{
    fogVolumes.push_back(v);
}

const std::vector<FogVolume>& EffectManager::getFog() const
{
    return fogVolumes;
}

