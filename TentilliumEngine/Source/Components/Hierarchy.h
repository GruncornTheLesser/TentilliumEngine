#pragma once
#include <entt/entt.hpp>


struct Hierarchy
{
friend class Scene;

private:
	unsigned int depth;
	entt::entity parent;
		
public:
	Hierarchy() : parent((entt::entity)0), depth(0) { }
	Hierarchy(entt::entity p) : parent(p), depth(0) { }

	inline entt::entity getParent()
	{
		return parent;
	}
};