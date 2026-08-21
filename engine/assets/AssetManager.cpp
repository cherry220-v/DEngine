#include "AssetManager.hpp"

#include <render/shader/Shader.hpp>
#include <render/texture/Material.hpp>
#include <render/texture/Texture.hpp>
#include <models/Model.hpp>

#include <ecs/Scene.hpp>

#include <iostream>
#include <stdexcept>
#include <typeinfo>

void AssetManager::scanAssets()
{
    if (m_assetsPath.empty() || !fs::exists(m_assetsPath)) return;

    for (const auto& entry : fs::recursive_directory_iterator(m_assetsPath)) {
        if (entry.is_regular_file() && entry.path().extension() != ".meta") {
            processEntry(entry);
        }
    }
}

void AssetManager::processEntry(const fs::directory_entry& entry)
{
    fs::path assetPath = entry.path();
    fs::path metaPath = assetPath.string() + ".meta";

    if (!fs::exists(metaPath))
    {
        generateMetaFile(assetPath);
    }

    std::ifstream f(metaPath);
    nlohmann::json j = nlohmann::json::parse(f);

    AssetMeta meta;
    meta.uuid = j.value("uuid", "");
    meta.path = assetPath.string();
    meta.asset_type = j.value("asset_type", "Unknown");
    meta.isAdditional = j.value("isAdditional", true);

    if (j.contains("dependencies"))
    {
        meta.dependencies = j["dependencies"]
            .get<std::vector<std::string>>();
    }

    m_metaRegistry[meta.uuid] = meta;
    m_assetToUuid[meta.path] = meta.uuid;
    m_uuidToAsset[meta.uuid] = meta.path;
}


void AssetManager::generateMetaFile(const fs::path& assetPath)
{
    fs::path metaPath = assetPath.string() + ".meta";

    std::string ext = assetPath.extension().string();

    auto it = AssetRegistry::get().assets().find(ext);

    AssetTypeInfo rule;
    if (it != AssetRegistry::get().assets().end())
        rule = it->second;
    else
        rule = AssetTypeInfo{ AssetType::Additional, nullptr };

    nlohmann::json metaJson;
    metaJson["uuid"] = GUUID::generate();
    metaJson["asset_type"] = static_cast<int>(rule.type);
    metaJson["isAdditional"] = (rule.type == AssetType::Additional);

    std::ofstream file(metaPath);
    file << metaJson.dump(4);

    AssetMeta meta;
    meta.uuid = metaJson["uuid"];
    meta.asset_type = static_cast<int>(rule.type);
    meta.path = assetPath.string();
    meta.isAdditional = (rule.type == AssetType::Additional);

    m_metaRegistry[meta.uuid] = meta;
}


void AssetManager::unloadUnusedAssets() {
    auto it = m_loadedAssets.begin();
    while (it != m_loadedAssets.end()) {
        long useCount = it->second.use_count();

        if (useCount <= 1) {
            it = m_loadedAssets.erase(it);
        }
        else {
            ++it;
        }
    }
}

void AssetManager::clearCache() {
    m_loadedAssets.clear();
}