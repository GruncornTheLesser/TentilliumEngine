#pragma once
#include "Components.h"
#include <list>

auto groupIncHierTran		= reg.group<Hierarchy, Transform>();
auto viewIncHier			= reg.view<Hierarchy>();
auto viewIncTranExcHier		= reg.view<Transform>(exclude<Hierarchy>);
auto viewIncHierExcTran		= reg.view<Hierarchy>(exclude<Transform>);

// I dont like the scene tag. like at all.
// Tags wont compile unless they're constants -> maybe runtime views will help?
// I need a way to isolate entities that belong to a scene.
// 

// I also need a naming convention



// VALIDATE HIERARCHY SYSTEM
// reorder hierarchy such that parents are iterated over before children
// tree builder algorithm thing:

// allocate array size as 2x number of entity
// for each entity include hierarchy:
//		if entity in roots array:
//			remove entity from roots
//			if not entity parent in root:
//				add entity parent to root
//			add to front of array
//		else
//			add to end of array
// copy array from allocated section

// BUILD RENDER SYSTEM
// i have no idea how to do this
// i thought maybe i could have a bunch of group observers for Transform, skinning etc
// i can obviously do just a flat 'if entity has component' but thats not very ECS
// 
// what does it need to do:
// when any relevant component is added or removed from an entity with the render component
// adjust the shader program to take only the uniform buffer inputs available to give
// 
// alternatively i could make it so the uniform buffer slots are always same for each shader program.
// then when a value isnt used its just ignored???


auto viewRender = reg.view<Render>();





class Scene
{
private:
	static inline Scene* instance;

	Scene() {}

public:
	static Scene* GetInstance()
	{
		if (!instance)
			instance = new Scene();
		return instance;
	}

	void Load(const char* filepath)
	{
		Clear();
	}
	void Clear()
	{
		reg.clear();
	}

	/// <summary>
	/// adds already existing entity to the scene and adds components to the entity. 
	/// if component already belongs to entity, original is preserved.
	/// </summary>
	template<class ... components>
	void Add(entity ent, components ... comps)
	{	
		((void)reg.get_or_emplace<components>(ent, comps), ...);	// add each component with copy constructor
	}

	template<class ... components>
	entity Create(components...comps)
	{
		auto ent = reg.create();
		(reg.emplace<components>(ent, comps), ...);
		return ent;
	}

	/// <summary>
	/// O(n^2)
	/// </summary>
	void Validate()
	{
		std::list<entity> arr;
		
		for (auto [ent, hierarchy] : viewIncHier.each())
		{

			bool Valid = false;
			for (entity e : arr)
			{
				if (e == hierarchy.parent)
				{
					Valid = true;
					break;
				}
			}
				
			if (Valid)
				arr.push_front(ent);
			else
				arr.push_back(ent);
		}

		int i = 0;
		for (entity e : arr)
		{
			i++;
			auto &h = reg.get<Hierarchy>(e);
			h.depth = i;
		}

		//reg.sort<Hierarchy>([](Hierarchy& lhs, Hierarchy& rhs) {lhs.depth > rhs.depth; });
	}

	void tick()
	{

		// recalculate hierarchy depth and check for orphaned children
		for (auto [entity, hierarchy] : viewIncHier.each())
		{
			auto temp = hierarchy.depth;

			if (!reg.valid(hierarchy.parent)) // invalid orphaned children moved to root
				hierarchy.depth = 0;		
			else if (auto parent = reg.try_get<Hierarchy>(hierarchy.parent))
				hierarchy.depth = 1 + parent->depth;
			else
				hierarchy.depth = 1;

			if (temp != hierarchy.depth)
				hierarchy.version = Hierarchy::currentVersion;
		}

		// update root transforms
		for (auto [entity, transform] : viewIncTranExcHier.each()) // transform and no hierarchy
		{
			if (transform.version == Transform::currentVersion)
			{
				transform.localMatrix =
					glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, transform.position.x, transform.position.y, transform.position.z, 1);// *
					//glm::mat4(transform.scale.x, 0, 0, 0, 0, transform.scale.y, 0, 0, 0, 0, transform.scale.z, 0, 0, 0, 0, 1);// *
					//(mat4)transform.rotation;
				transform.worldMatrix = transform.localMatrix;
			}
			
		}

		// update transform world matrix
		for (auto [entity, hierarchy, transform] : groupIncHierTran.each()) // transform and hierarchy
		{
			if (transform.version == Transform::currentVersion || hierarchy.version == Hierarchy::currentVersion)
			{
				transform.localMatrix =
					glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, transform.position.x, transform.position.y, transform.position.z, 1);// *
					//glm::mat4(transform.scale.x, 0, 0, 0, 0, transform.scale.y, 0, 0, 0, 0, transform.scale.z, 0, 0, 0, 0, 1);// *
					//(mat4)transform.rotation;

				if (hierarchy.depth == 0)
				{
					transform.worldMatrix = transform.localMatrix;
					reg.erase<Hierarchy>(entity);
				}
				else if (auto parent = reg.try_get<Transform>(hierarchy.parent))			// if parent has transform 
					transform.worldMatrix = parent->worldMatrix * transform.localMatrix;	// use parent world transform as root
				else
					transform.worldMatrix = transform.localMatrix;							// use local transform as root
			}
		}

		for (auto [entity, hierarchy] : viewIncHierExcTran.each())
			if (hierarchy.depth == 0)
				reg.erase<Hierarchy>(entity);

		Hierarchy::currentVersion++;
		Transform::currentVersion++;

		// for testing
		for (auto [entity, hierarchy, transform] : groupIncHierTran.each())
			std::cout << (int)entity << ": parent = " << (int)hierarchy.parent << "[Has Transform]" << std::endl;

		for (auto [entity, hierarchy] : viewIncHierExcTran.each())
			std::cout << (int)entity << ": parent = " << (int)hierarchy.parent << "[No Transform]" << std::endl;

	}
};
