#pragma once

#include <assets/ILoader.hpp>
#include <render/texture/Texture.hpp>

#include <assets/AssetRegistry.hpp>

class TextureLoader : public ILoader
{
public:
    std::shared_ptr<IAsset> load(
        const std::string& path
    ) override;
};

