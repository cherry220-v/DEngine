#include "AssetRegistry.hpp"

void AssetRegistry::registerLoader(const std::string& extension, AssetTypeInfo info)
{
    m_loaders[extension] =
        std::move(info);
}

void AssetRegistry::registerLoader(std::vector<std::string> extensions, AssetTypeInfo info)
{
    for (auto& ext : extensions)
        registerLoader(ext, info);
}

AssetTypeInfo AssetRegistry::getTypeInfo(const std::string& extension)
{
    return m_loaders.find(extension)->second;
}

ILoader* AssetRegistry::getLoader(
    const std::string& extension)
{
    if (extension.length() <= 0) return nullptr;
    auto it =
        m_loaders.find(extension);

    if (it == m_loaders.end())
    {
        throw std::runtime_error(
            "No loader registered for " +
            extension);
    }

    return it->second.loader.get();
}

const std::unordered_map<std::string, AssetTypeInfo>& AssetRegistry::assets() const
{
    return m_loaders;
}
