#pragma once
#include <cstdint>
#include <string>

using Entity = uint32_t;
constexpr unsigned int NullEntity = 0;;
constexpr Entity INVALID_ENTITY = 0;

struct EntityInfo
{
    Entity id;
    std::string uuid;
};