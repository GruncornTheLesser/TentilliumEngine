#pragma once
#include <entt/entt.hpp>
using namespace entt;
struct Hierarchy
{
public:
	entity parent;
	unsigned int depth;
	unsigned int version;

	static inline unsigned int currentVersion = 0;
	
public:
	Hierarchy() : version(currentVersion) { } // creates a root hierarchy component
	Hierarchy(entity p) : parent(p), version(currentVersion) { }
};