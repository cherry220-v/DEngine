#include "ShaderLoader.hpp"

#include <render/shader/Shader.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

#include <assets/AssetManager.hpp>

std::shared_ptr<IAsset> ShaderLoader::load(
    const std::string& path)
{
    auto shader =
        std::make_shared<Shader>();

    auto& assetManager =
        AssetManager::get();

    std::ifstream file(path);

    if (!file.is_open())
    {
        throw std::runtime_error(
            "Could not open shader file: " + path);
    }

    nlohmann::json data =
        nlohmann::json::parse(file);

    std::string vertexUuid =
        data.at("vertex")
        .get<std::string>();

    std::string fragmentUuid =
        data.at("fragment")
        .get<std::string>();

    shader->m_name =
        data.value(
            "name",
            "Unnamed Shader"
        );

    std::string vertexPath =
        assetManager.getPathByUuid(
            vertexUuid);

    std::string fragmentPath =
        assetManager.getPathByUuid(
            fragmentUuid);

    std::string vertexCode =
        loadFile(vertexPath);

    std::string fragmentCode =
        loadFile(fragmentPath);

    unsigned int vertex =
        shader->compileShader(
            GL_VERTEX_SHADER,
            vertexCode
        );

    unsigned int fragment =
        shader->compileShader(
            GL_FRAGMENT_SHADER,
            fragmentCode
        );

    shader->linkProgram(
        vertex,
        fragment
    );

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return shader;
}

std::string ShaderLoader::loadFile(
    const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        throw std::runtime_error(
            "Failed to open file: " + path);
    }

    std::stringstream ss;
    ss << file.rdbuf();

    return ss.str();
}