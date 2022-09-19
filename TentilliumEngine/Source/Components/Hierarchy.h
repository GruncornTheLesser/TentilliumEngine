#pragma once
#include <entt/entt.hpp>

struct Hierarchy
{
friend class TransformSystem;

public:
	entt::entity parent;

public:
	Hierarchy(entt::entity p) : parent(p) { }
};