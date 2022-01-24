#pragma once
#include <entt/entt.hpp>
using namespace entt;
struct Hierarchy
{
public:
	entity parent;
	int version;
	unsigned int depth;
public:
	Hierarchy() {} // creates a root hierarchy component
	Hierarchy(entity p) : parent(p) { }
};