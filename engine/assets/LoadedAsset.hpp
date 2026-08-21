#pragma once

#include "IAsset.hpp"
#include "AssetMeta.hpp"

#include <memory>

struct LoadedAsset
{
    AssetMeta meta;
    std::shared_ptr<IAsset> object;
};