#pragma once

#include <nlohmann/json.hpp>
#include <vector>
#include <string>

struct AssetMeta
{
    std::string name;

    std::string uuid;
    std::string path;

    bool isAdditional = false;

    std::string asset_type;
    std::vector<std::string> dependencies;
};
