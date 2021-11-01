#pragma once
#include <ECS.h>
#include "Shader.h"
#include "Mesh.h"
#include "AppWindow.h"

using entityID = uint32_t;

struct RenderComponent
{
	Mesh* mesh;
	Shader* shader;
	RenderComponent() : mesh(nullptr), shader(nullptr) { }
	RenderComponent(Mesh* mesh, Shader* shader) : mesh(mesh), shader(shader) { }
	RenderComponent(RenderComponent& orig) : mesh(orig.mesh), shader(orig.shader) { }
	RenderComponent& operator=(const RenderComponent& other) 
	{
		if (this == &other) return *this;
		shader = other.shader;
		mesh = other.mesh;
		return *this;
	}
};

struct LightComponent { };
struct OccluderComponent { };


Registry<entityID, Archetype_Set_builder
	::add<Archetype<>>
	::add<Archetype<RenderComponent>>
	::build> entityReg; // entity registry

// wrapper class
class Entity
{
private:
	entityID m_ID;

public:
	template<class ... Components>
	Entity(const Components&... comps) : m_ID(entityReg.create<Components...>(comps...)) { }
	~Entity()
	{
		entityReg.destroy(m_ID);
	}

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
