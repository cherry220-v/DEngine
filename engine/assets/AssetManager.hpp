#pragma once
#include "AssetRegistry.hpp"
#include "AssetLoaderRegistration.hpp"

#include <render/shader/Shader.hpp>

#include <unordered_set>
#include <string>
#include <memory>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <core/GUUID.h>

namespace fs = std::filesystem;

class AssetManager
{
public:
	static AssetManager& get()
	{
		static AssetManager instance;
		return instance;
	}
	AssetManager(const AssetManager&) = delete;
	AssetManager& operator=(const AssetManager&) = delete;

public:
	void setAssetsPath(const std::string& path) { m_assetsPath = path; }
	void scanAssets();

	void unloadUnusedAssets();
	void clearCache();

	template<typename T>
	std::shared_ptr<T> load(const std::string& uuid)
	{
		if (uuid.length() <= 0) return nullptr;
		auto it = m_loadedAssets.find(uuid);

		if (it != m_loadedAssets.end())
		{
			return std::dynamic_pointer_cast<T>(
				it->second
			);
		}

		auto path =
			getPathByUuid(uuid);

		auto loader =
			AssetRegistry::get()
			.getLoader(
				fs::path(path)
				.extension()
				.string()
			);

		auto asset =
			loader->load(path);

		asset->meta = getMetaByUuid(uuid);

		m_loadedAssets[uuid] = asset;

		return std::dynamic_pointer_cast<T>(
			asset
		);
	}

	template<typename T>
	void add(std::string name, std::shared_ptr<T> asset)
	{
		//if (!asset)
		//    throw std::runtime_error("Trying to add null asset: " + name);

		//m_loadedAssets[name] = asset;
	}

	bool assetsLoaded() { return m_assetsLoaded; }
	
	std::string getUuidByPath(const std::string& path) { auto it = m_assetToUuid.find(path); if (it == m_assetToUuid.end()) { return ""; } else { return m_assetToUuid.at(path); } };
	std::string getUuidByAsset(std::shared_ptr<IAsset> asset) { return asset->meta->uuid; };
	std::string getPathByUuid(const std::string& uuid) { auto it = m_uuidToAsset.find(uuid); if (it == m_uuidToAsset.end()) { return ""; } else { return m_uuidToAsset.at(uuid); } };
	AssetMeta* getMetaByUuid(const std::string& uuid) {
		auto it = m_metaRegistry.find(uuid);
		if (it == m_metaRegistry.end()) {
			return nullptr;
		}
		return &m_metaRegistry.at(uuid);
	}

	std::shared_ptr<IAsset> getAssetByUuid(const std::string& uuid)
	{
		auto it = m_loadedAssets.find(uuid);
		if (it == m_loadedAssets.end()) return std::shared_ptr<IAsset>();
		return it->second;
	}
private:
	AssetManager() = default;

	std::string m_assetsPath;

	bool m_assetsLoaded = false;

	std::unordered_map<
		std::string,
		std::shared_ptr<IAsset>
	> m_loadedAssets;
	std::unordered_map<std::string, AssetMeta> m_metaRegistry;
	std::unordered_map<std::string, std::string> m_assetToUuid;
	std::unordered_map<std::string, std::string> m_uuidToAsset;

	void processEntry(const fs::directory_entry& entry);
	void generateMetaFile(const fs::path& assetPath);
};