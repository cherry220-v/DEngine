#pragma once

#include <memory>

#include "IAsset.hpp"
#include "ILoader.hpp"

enum class AssetType
{
    Shader,
    Texture,
    Model,
    Material,
    Scene,
    Prefab,
    Script,
    Additional
};

struct AssetTypeInfo
{
    AssetType type;
    std::shared_ptr<ILoader> loader;
    std::function<void(
        void* destination,
        std::shared_ptr<IAsset> asset
        )> assign;
};

template<typename T>
inline AssetTypeInfo makeAssetTypeInfo(
    AssetType type,
    std::shared_ptr<ILoader> loader)
{
    return {
        type,
        loader,

        [](void* dst, std::shared_ptr<IAsset> asset)
        {
            *reinterpret_cast<std::shared_ptr<T>*>(dst) =
                std::dynamic_pointer_cast<T>(asset);
        }
    };
}