#pragma once

#include <memory>
#include <string>

#include "AssetManager.hpp"
#include "IAssetRef.hpp"

template<typename T>
class AssetRef : public IAssetRef
{
public:
    AssetRef() = default;

    AssetRef(const std::string& uuid)
        : m_uuid(uuid)
    {
    }

public:
    std::string m_uuid;

    std::weak_ptr<T> cached;

    std::shared_ptr<T> get()
    {
        if (m_uuid.empty())
            return nullptr;

        if (auto ptr = cached.lock())
            return ptr;

        auto asset =
            AssetManager::get().load<T>(m_uuid);

        cached = asset;

        return asset;
    }

    const std::string& uuid() override { return m_uuid; }

    void setUuid(
        const std::string& value
    ) override
    {
        m_uuid = value;
        cached.reset();
    }

    operator bool() const
    {
        return !m_uuid.empty();
    }

    void reset()
    {
        m_uuid.clear();
        cached.reset();
    }
};