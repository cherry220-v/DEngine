#pragma once

#include "IComponent.hpp"

#include <animation/Skeleton.hpp>
#include <memory>

#include <assets/AssetRef.hpp>

struct SkeletonComponent : IComponent
{
    AssetRef<Skeleton> skeleton;
};