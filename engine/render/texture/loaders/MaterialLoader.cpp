#include "MaterialLoader.hpp"
#include <assets/AssetManager.hpp>

std::shared_ptr<IAsset> MaterialLoader::load(const std::string& path)
{
    auto& assetManager = AssetManager::get();

    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open material file: " + path);
    }

    nlohmann::json data = nlohmann::json::parse(file);
    std::string shaderUuid = data.at("shader").get<std::string>();
    std::string textureUuid = data.at("texture").get<std::string>();

    auto material = std::make_shared<Material>();

    material->m_name = data.value("name", "Unnamed Material");

    material->shader = assetManager.load<Shader>(shaderUuid);
    material->texture = assetManager.load<Texture>(textureUuid);
    return material;
}