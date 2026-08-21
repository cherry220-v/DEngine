#pragma once

#include <assets/ILoader.hpp>
#include <vector>
#include <unordered_map>
#include <tiny_gltf.h>
#include <memory>

#include <assets/AssetRegistry.hpp>

class GLBLoader : public ILoader
{
public:
	std::shared_ptr<IAsset> load(const std::string& path) override;
};