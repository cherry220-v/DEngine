#pragma once

#include "IComponent.hpp"

#include <core/GUUID.h>

#include <string>

struct IDComponent : public IComponent
{
	std::string id;
};