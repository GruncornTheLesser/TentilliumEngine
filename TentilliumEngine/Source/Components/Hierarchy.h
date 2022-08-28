#pragma once
#include <entt/entt.hpp>
#include "Flag.h"

struct Hierarchy
{
friend class TransformSystem;

public:
	entt::entity parent;

public:
	Hierarchy(entt::entity p) : parent(p) { }
};