#pragma once

#include "Skeleton.hpp"
#include "Animation.hpp"
#include <vector>
#include <memory>

class Model;

struct SkinnedModel
{
    std::shared_ptr<Model> model;
    std::vector<Skeleton> skeletons;
    std::vector<AnimationClip> animations;
};