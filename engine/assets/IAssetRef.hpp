#pragma once

#include <string>

class IAssetRef
{
public:
    virtual ~IAssetRef() = default;

    virtual const std::string& uuid() = 0;

    virtual void setUuid(
        const std::string& uuid
    ) = 0;
};