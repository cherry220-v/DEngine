#pragma once

#include "SkinnedModel.hpp"

struct SkinnedInstance
{
    SkinnedModel* asset;
    int animationIndex;
    float time;
};