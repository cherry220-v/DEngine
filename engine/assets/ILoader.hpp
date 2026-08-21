#pragma once

#include <assets/IAsset.hpp>

#include <string>
#include <memory>

class ILoader
{
public:
    virtual ~ILoader() = default;

    virtual std::shared_ptr<IAsset>
        load(const std::string& path) = 0;
};