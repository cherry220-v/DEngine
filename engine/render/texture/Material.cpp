#include "Material.hpp"

#include "Texture.hpp"
#include "../shader/Shader.hpp"

void Material::bind()
{
    shader->use();

    shader->setVec4("objectColor", color);

    if (texture)
    {
        texture->bind(0);
        shader->setInt("texture1", 0);
        shader->setBool("useTexture", true);
    }
    else
    {
        shader->setBool("useTexture", false);
    }
}