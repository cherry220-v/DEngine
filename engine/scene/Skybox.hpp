#pragma once

#include <memory>
#include <vector>
#include <string>

#include <glm/gtc/matrix_transform.hpp>
#include <render/texture/Material.hpp>

class Skybox
{
public:
    Skybox(std::shared_ptr<Material> material,
        const std::vector<std::string>& faces);

    void draw(const glm::mat4& view, const glm::mat4& projection);

private:
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int cubemap = 0;

    std::shared_ptr<Material> material;

    void setupMesh();
    unsigned int loadCubemap(const std::vector<std::string>& faces);
};