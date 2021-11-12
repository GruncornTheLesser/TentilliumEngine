#pragma once
#include <ECS.h>
#include "Shader.h"
#include "Mesh.h"
#include "AppWindow.h"

using entityID = uint32_t;

/*
struct TransformComponent { };
struct HierarchyComponent
{
private:
	const int m_depth;
public:
	const int m_parent;

	HierarchyComponent() : m_depth(), m_parent() { };
	// 2 systems -> 1 to set parent and 1 to update transform
	// set parent -> recalculate heirachy calcuation order, ie inserts component and children into new position in array
	// calculate breadth first graph which means just iterate down the array so the top gets updated first
};

struct LightComponent { };
struct OccluderComponent { };


*/
class RenderComponent 
{
private:
	unsigned int m_vao;
	unsigned int m_length;
	RenderComponent() 
		: m_vao(0), m_length(0) { }
public:
	RenderComponent(const char* filepath)
	{

	}
	void Draw()
	{

	}
};
class ShadowComponent 
{ 
private:
	unsigned int m_vao;
	unsigned int m_length;
	ShadowComponent()
		: m_vao(0), m_length(0) { }
public:
	ShadowComponent(const char* filepath)
	{

	}
	void Draw()
	{

	}
};


struct CustomShader : Shader { };
struct SkeletonComponent { };
// entity registry
Registry<entityID, Archetype_Set_builder // TODO: need a better way to build registry
	::add<Archetype<RenderComponent>>
	::add<Archetype<RenderComponent, CustomShader>>
	::build> entityReg;

// wrapper class
class Entity
{
private:
	const entityID m_ID;

public:
	template<class ... Components>
	Entity(const Components&... comps) : m_ID(entityReg.create<Components...>(comps...)) { }
	~Entity() { entityReg.destroy(m_ID); }


	template <class Component>
	bool Has()
	{
		return entityReg.has<Component>(m_ID);
	}

	template <class Component>
	Component& Get()
	{
		return entityReg.unpack<Component>(m_ID);
	}

	template <class ... Components>
	void Set()
	{
		entityReg.swap_Archetype<Components>(m_ID);
	}
};
