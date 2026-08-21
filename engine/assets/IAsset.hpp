#pragma once

#include "AssetMeta.hpp"

class IAsset
{
public:
    AssetMeta* meta = nullptr;
    virtual ~IAsset() = default;
};