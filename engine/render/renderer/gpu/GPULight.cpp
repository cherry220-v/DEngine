#include "GPULight.hpp"

#include <algorithm>
#include <string>

#include <render/shader/Shader.hpp>

namespace
{
constexpr int MaxUploadedLights = 8;
}

void uploadLights(Shader& shader, const std::vector<GPULight>& lights)
{
    shader.use();

    const int count = std::min(static_cast<int>(lights.size()), MaxUploadedLights);
    shader.setInt("lightCount", count);

    for (int i = 0; i < count; i++)
    {
        const auto& light = lights[i];
        const auto index = std::to_string(i);

        shader.setVec3("lightColors[" + index + "]", { light.color.x, light.color.y, light.color.z });
        shader.setFloat("lightIntensities[" + index + "]", light.color.w);

        const int type = static_cast<int>(light.params.x);
        shader.setInt("lightTypes[" + index + "]", type);

        if (type == 0)
        {
            shader.setVec3("lightDirections[" + index + "]", { light.direction.x, light.direction.y, light.direction.z });
        }
        else if (type == 1)
        {
            shader.setVec3("lightPositions[" + index + "]", { light.position.x, light.position.y, light.position.z });
        }
        else if (type == 2)
        {
            shader.setVec3("lightPositions[" + index + "]", { light.position.x, light.position.y, light.position.z });
            shader.setVec3("lightDirections[" + index + "]", { light.direction.x, light.direction.y, light.direction.z });
        }
    }
}
