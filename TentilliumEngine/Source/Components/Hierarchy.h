#pragma once
#include <entt/entt.hpp>


struct Hierarchy
{
friend class Scene;

private:
	entt::entity parent;
	
public:
	Hierarchy() { }
	Hierarchy(entt::entity p) : parent(p) { }

	entt::entity getParent()
	{
		return parent;
	}
};