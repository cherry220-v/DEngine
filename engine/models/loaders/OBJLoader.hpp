#pragma once

#include <assets/ILoader.hpp>
#include <vector>
#include <unordered_map>

#include <assets/AssetRegistry.hpp>

class OBJLoader : public ILoader
{
public:
	std::shared_ptr<IAsset> load(const std::string& path) override;
};