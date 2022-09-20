#pragma once
#include <entt/entt.hpp>

struct Parent
{
friend class TransformSystem;

public:
	entt::entity parent;

public:
	Parent(entt::entity p) : parent(p) { }
};