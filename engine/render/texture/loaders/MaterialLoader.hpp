#pragma once

#include <assets/ILoader.hpp>
#include <render/texture/Material.hpp>

#include <assets/AssetRegistry.hpp>

class MaterialLoader : public ILoader
{
public:
    std::shared_ptr<IAsset> load(
        const std::string& path
    ) override;
};