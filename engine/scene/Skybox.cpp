#include "Skybox.hpp"

#include <glad/glad.h>
#include <stb_image.h>
#include <stdexcept>

#include <render/texture/Material.hpp>
#include <render/shader/Shader.hpp>
#include <filesystem>

static float skyboxVertices[] = {
    -1,-1,-1,  1,-1,-1,  1, 1,-1,  1, 1,-1, -1, 1,-1, -1,-1,-1,
    -1,-1, 1, -1,-1,-1, -1, 1,-1, -1, 1,-1, -1, 1, 1, -1,-1, 1,
     1,-1,-1,  1,-1, 1,  1, 1, 1,  1, 1, 1,  1, 1,-1,  1,-1,-1,
    -1,-1, 1, -1, 1, 1,  1, 1, 1,  1, 1, 1,  1,-1, 1, -1,-1, 1,
    -1, 1,-1,  1, 1,-1,  1, 1, 1,  1, 1, 1, -1, 1, 1, -1, 1,-1,
    -1,-1,-1, -1,-1, 1,  1,-1,-1,  1,-1,-1, -1,-1, 1,  1,-1, 1
};

Skybox::Skybox(std::shared_ptr<Material> material,
    const std::vector<std::string>& faces)
    : material(std::move(material))
{
    setupMesh();
    cubemap = loadCubemap(faces);
}

void Skybox::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(skyboxVertices),
        skyboxVertices,
        GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

unsigned int Skybox::loadCubemap(const std::vector<std::string>& faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    GLint maxSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(
            faces[i].c_str(),
            &width, &height, &nrChannels, 0
        );

        if (data)
        {
            GLenum internalFormat;
            GLenum dataFormat;

            if (nrChannels == 4)
            {
                internalFormat = GL_RGBA;
                dataFormat = GL_RGBA;
            }
            else if (nrChannels == 3)
            {
                internalFormat = GL_RGB;
                dataFormat = GL_RGB;
            }
            else
            {
                stbi_image_free(data);
                continue;
            }

            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                internalFormat,
                width,
                height,
                0,
                dataFormat,
                GL_UNSIGNED_BYTE,
                data
            );
            GLenum err = glGetError();
            if (err != GL_NO_ERROR)
            {
            }

            stbi_image_free(data);
        }
        if (!data)
        {
            continue;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void Skybox::draw(const glm::mat4& view, const glm::mat4& projection)
{
    auto& shader = material->shader;
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    glDisable(GL_CULL_FACE);

    shader->use();

    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));

    shader->setMat4("view", viewNoTranslation);
    shader->setMat4("projection", projection);

    glBindVertexArray(VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    shader->setInt("skybox", 0);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);

    glEnable(GL_CULL_FACE);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}