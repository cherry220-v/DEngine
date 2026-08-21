#pragma once

#include "SkinnedModel.hpp"
#include "Animation.hpp"
#include <memory>

struct ModelAnimation
{
    std::shared_ptr<SkinnedModel> model;
    std::shared_ptr<AnimationClip> clip;
    float time = 0.0f;
};