#pragma once

#include <assets/ILoader.hpp>
#include <string>

#include <assets/AssetRegistry.hpp>

class ShaderLoader : public ILoader
{
public:
	std::shared_ptr<IAsset> load(const std::string& path) override;

	std::string loadFile(const std::string& path);
};