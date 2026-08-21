#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/mat4x4.hpp>

#include "Bone.hpp"

struct Skeleton
{
    std::vector<Bone> bones;

    std::unordered_map<std::string, int> boneMap;

    int rootBone = -1;

    int getBoneIndex(const std::string& name) const
    {
        auto it = boneMap.find(name);
        if (it == boneMap.end()) return -1;
        return it->second;
    }
};