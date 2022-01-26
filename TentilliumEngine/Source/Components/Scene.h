#pragma once
#include "Components.h"
template<int sceneID = 0>
struct SceneTag {};

template<int sceneID>
auto sceneView				= reg.view<SceneTag<sceneID>>();
template<int sceneID>
auto groupSceneGraph		= reg.group<SceneTag<sceneID>, Hierarchy, Transform>();
template<int sceneID>
auto viewHierarchy			= reg.view<SceneTag<sceneID>, Hierarchy>();
template<int sceneID>
auto viewTransformRoot		= reg.view<SceneTag<sceneID>, Transform>(exclude<Hierarchy>);




/*
// reactive transform hierarchy system

could use version ids
	when you change a transform value update the version
	iterate over transforms looking for where update happened in current version

maybe observer will preserve grouping order??? that would be rly fuckin convenient -> it doesnt
	ENTT docs - "In general, observers don't stay true to the order of any set of components"

could add tag when value has changed and iterate over a view




// user patches transform to change transform values
// user adds or removes hierarchy component to change hierarchy values





// reactive system with versions ids
for each Hierarchy:
	temp = Hierarchy.depth

	if not Hierarchy.parent exists:
		Hierarchy.depth = 0
	else if Hierarchy.parent has Hierarchy:
		Hierarchy.depth = Hierarchy.parent->Hierarchy.depth + 1
	else:
		Hierarchy.depth = 1

	if not temp == Hierarchy.depth:
		Hierarchy.version = currentVersion

for each entity include Transform, exclude Hierarchy:
	if Transform.version == currentVersion:
		Transform.local = { recaclulate Transform matrix }
		Transform.world = Transform.local

for each entity include Transform, include Hierarchy:
	if Transform.version == currentVersion or Hierarchy.version == currentVersion:
		Transform.local = { recaclulate Transform matrix }

		if Hierarchy.parent has Transform:
			Transform.world = Transform.world * Transform.local
		else:
			Transform.world = Transform.local

for each entity include Hierarchy:
	if Hierarchy.depth == 0:
		remove Hierarchy component

	












// non reactive transform hierarchy system

for each entity with Hierarchy:
	if not Hierarchy.parent exists:
		remove hierarchy from entity;

	if Hierarchy.parent has Hierarchy:
		Hierarchy.depth = Hierarchy.parent->Hierarchy.depth + 1
	else:
		Hierarchy.depth = 1

for each entity with Transform and not Hierarchy:
	Transform.local = { calculate local transform }
	Transform.world = Transform.local

for each entity with Transform and Hierarchy:
	Transform.local = { calculate local transform }

	if Hierarchy.parent has Transform:
		Transform.world = Transform.world * Transform.local
	else:
		Transform.world = Transform.local

*/

template<int sceneID>
auto viewRender = reg.view<SceneTag<sceneID>, Render>();


template<int sceneID>
class Scene
{
public:
	/// <summary>
	/// adds already existing entity to the scene and adds components to the entity. 
	/// if component already belongs to entity, original is preserved.
	/// </summary>
	template<class ... components>
	static void Add(entity ent, components ... comps)
	{
		reg.emplace_or_replace<SceneTag<sceneID>>(ent);				// mark it to belong to this scene
		((void)reg.get_or_emplace<components>(ent, comps), ...);	// add each component with copy constructor
	}

	template<class ... components>
	static entity Create(components...comps)
	{
		auto ent = reg.create();
		(reg.emplace<components>(ent, comps), ...);
		return ent;
	}

	static void HierarchySystem()
	{
		// TODO: could try and identify updated hierarchy entities in an observer to be used by transform system
		// PROS: could allow transform and other systems to update only those that changed
		// CONS: much more complex
		for (auto [entity, hierarchy] : viewHierarchy<sceneID>.each())
		{
			if (!reg.valid(hierarchy.parent)) // invalid orphaned children moved to root
			{
				reg.erase<Hierarchy>(entity);
				continue;
			}
				
			else if (auto parent = reg.try_get<Hierarchy>(hierarchy.parent))
				hierarchy.depth = 1 + parent->depth;
			else
				hierarchy.depth = 1;
		}
	}

	static void ValidateHierarchySystem()
	{
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

	}

	static void TransformSystem()
	{
		// TODO: could create reactive system by updating local matrix in an observer
		// PROS: faster, only updates components that change
		// CONS: requires use to update with a patch or replace call, more complicated, requires more views/observers/...


		// update root transforms
		for (auto [entity, transform] : viewTransformRoot<sceneID>.each()) // transform and no hierarchy
		{
			transform.localMatrix =
				glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, transform.position.x, transform.position.y, transform.position.z, 1);// *
				//glm::mat4(transform.scale.x, 0, 0, 0, 0, transform.scale.y, 0, 0, 0, 0, transform.scale.z, 0, 0, 0, 0, 1);// *
				//(mat4)transform.rotation;
			transform.worldMatrix = transform.localMatrix;
		}

		// update transform world matrix
		for (auto [entity, hierarchy, transform] : groupSceneGraph<sceneID>.each()) // transform and hierarchy
		{
			transform.localMatrix =
				glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, transform.position.x, transform.position.y, transform.position.z, 1);// *
				//glm::mat4(transform.scale.x, 0, 0, 0, 0, transform.scale.y, 0, 0, 0, 0, transform.scale.z, 0, 0, 0, 0, 1);// *
				//(mat4)transform.rotation;

			if (auto parent = reg.try_get<Transform>(hierarchy.parent)) // if parent has transform 
				transform.worldMatrix = parent->worldMatrix * transform.localMatrix; // use parent world transform as root
			else
				transform.worldMatrix = transform.localMatrix;			// use local transform as root
		}
	}

	static void BuildRenderSystem()
	{
		// reactive system
		// with a tag?
		// with a group observer?
		// with a version id?
	}

	static void RenderSystem()
	{
		// Model			// contained in vbo
		// mesh				// contained in ibo
		// material			// contained in ubo
		//									vao???
		// Transform		// contained in ubo
		// camera			// contained in ubo
		// light			// contained in ubo
		// atmosphere		// contained in ubo
		// skeleton data	// contained in ubo
		
		// normal forward rendering
		// camera.bind()
		// for each entity include Render:
		//		if includes Transform:
		//			Transform.bind()
		//		if includes Skinning:
		//			Skinning.bind()
		//		...
		// 
		// 
		//		for mesh in Render:
		//			material.bind()
		//			mesh.render()
		//			
	}

	

};
