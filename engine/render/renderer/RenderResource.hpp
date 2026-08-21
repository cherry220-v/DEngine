#pragma once

#include <memory>
#include <typeindex>

using RenderHandle = uint32_t;

struct RenderResource
{
    RenderResource()
        : data(nullptr), type(typeid(void)) {
    }

    std::shared_ptr<void> data;
    std::type_index type;
};