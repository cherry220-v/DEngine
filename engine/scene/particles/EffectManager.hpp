#pragma once

#include <vector>
#include <memory>
#include <iostream>

#include "FogVolume.hpp"

class Shader;
class Material;

class EffectManager {
public:
    EffectManager() = default;
    EffectManager(const EffectManager&) = default;
    EffectManager& operator=(const EffectManager&) = default;
    EffectManager(EffectManager&&) noexcept = default;
    EffectManager& operator=(EffectManager&&) noexcept = default;

public:
    void updateFog(float dt);

    void update(float dt);

    void addFogVolume(const FogVolume& v);

    const std::vector<FogVolume>& getFog() const;


    std::vector<FogVolume> fogVolumes;
};