#pragma once
#include <entt/entt.hpp>

struct Parent
{
friend class TransformSystem;

public:
	Parent(entt::entity p) : m_parent(p) { }

	operator entt::entity() { return m_parent; }

private:
	entt::entity m_parent;
};