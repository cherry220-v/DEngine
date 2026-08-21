#pragma once

#include "AssetMeta.hpp"
#include "AssetTypeInfo.hpp"
#include <vector>

class ILoader;

class AssetRegistry
{
public:
    static AssetRegistry& get() { static AssetRegistry instance; return instance; }

    void registerLoader(const std::string& extension, AssetTypeInfo info);
    void registerLoader(std::vector<std::string> extensions, AssetTypeInfo info);

    AssetTypeInfo getTypeInfo(const std::string& extension);
    ILoader* getLoader(const std::string& extension);

    const std::unordered_map<std::string, AssetTypeInfo>& assets() const;

private:
    std::unordered_map<
        std::string,
        AssetTypeInfo
    > m_loaders;
};
