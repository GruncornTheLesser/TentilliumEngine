#pragma once
#include "entt_macros.h"

class LoadSystem : virtual protected entt::registry {
public:
	entt::entity load(std::string filepath);
};