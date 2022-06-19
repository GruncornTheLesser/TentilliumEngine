#pragma once
#include <entt/entt.hpp>


struct Hierarchy
{
friend class TransformSystem;

private:
	entt::entity parent;
		
public:
	Hierarchy(entt::entity p) : parent(p) { }

	inline entt::entity getParent()
	{
		return parent;
	}
};