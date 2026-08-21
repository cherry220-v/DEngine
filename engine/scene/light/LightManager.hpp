#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include "Light.hpp"
#include "DirectionalLight.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"


#define MAX_LIGHTS 8

class LightManager {
public:
    LightManager() = default;
    LightManager(const LightManager&) = default;
    LightManager& operator=(const LightManager&) = default;
    LightManager(LightManager&&) noexcept = default;
    LightManager& operator=(LightManager&&) noexcept = default;

    std::vector<std::shared_ptr<Light>> lights;

    void addLight(const std::shared_ptr<Light>& light);

    std::vector<std::shared_ptr<Light>> getActiveLights() {
        if (lights.size() <= MAX_LIGHTS)
            return lights;

        return std::vector<std::shared_ptr<Light>>(
            lights.begin(),
            lights.begin() + MAX_LIGHTS
        );
    }

    std::shared_ptr<DirectionalLight> getDirectionalLight();
};