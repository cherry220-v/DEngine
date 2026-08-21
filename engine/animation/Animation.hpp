#pragma once

#include "KeyFrame.hpp"
#include <assets/IAsset.hpp>

#include <vector>
#include <unordered_map>

struct BoneAnimation
{
    std::vector<KeyFrame> keys;
};

struct AnimationClip : public IAsset
{
    float duration;
    std::unordered_map<int, BoneAnimation> channels;
};

struct AnimationState
{
    float time = 0.0f;
    float speed = 1.0f;
    bool loop = true;

    std::shared_ptr<AnimationClip> clip;
};

struct BonePose
{
    glm::mat4 transform;
};