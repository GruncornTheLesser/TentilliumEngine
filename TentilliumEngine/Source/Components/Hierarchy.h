#pragma once
#include <entt/entt.hpp>
#include "Flag.h"

struct Hierarchy
{
friend class TransformSystem;

public:
	entt::entity m_parent;

public:
	Hierarchy(entt::entity p) : m_parent(p) { }
};