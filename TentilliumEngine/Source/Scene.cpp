#include "Scene.h"
#include "Components/FlagManager.h"
#include <iostream>
#include <glm.hpp>
//#include <gtc/matrix_transform.hpp>

void Scene::HierarchyUpdate()
{
	// use cases:
	// user removes parent												// hierarchy must be removed
	// user adds hierarchy with parent to pre-existing hierarchy		// must be added to calculate after parent and is
	// user adds hierarchy with parent to not yet initiated hierarchy	// must be added to calculate after parent -> fucks up
	// user adds hierarchy with parent to entity						// no dependency
	// user sets hierarchy parent to more deep branch					// fucks up order -> entity order needs to be changed
	// user sets hierarchy parent to less deep branch					// maintains correct order
	// user removes hierarchy											// no problems

	// compare entity.depth to parent.depth + 1							// can use <=> operator
	// if the depth is greater, flag depth update						// change detected, valid order
	// if the depth is less, push to back of group, flag depth update	// change detected, invalid order -> only happens when entity's parent changed to deeper entity
	// if the depth is equal, do nothing								// no change detected
	// new loop
	// set entity.depth = parent.depth + 1								// update depth for next time
	
	// use invalid depth to flag if update needed

	for (auto [entity, hierarchy] : viewHierarchy.each())
	{
		if (!valid(hierarchy.parent))
		{
			erase<Hierarchy>(entity);
			continue;
		}
		
		hierarchy.depth = 1;
		if (auto parent = try_get<Hierarchy>(hierarchy.parent))
			hierarchy.depth += parent->depth;
	}

	// simplest solution -> doesnt necessarily work
	groupSceneGraph.sort<Hierarchy>([](const auto& lhs, const auto& rhs) { return lhs.depth < rhs.depth; });
}

void Scene::TransformUpdate()
{
	for (auto [entity, transform] : viewRootTransform.each())
	{
		// if updated locally, update local and world transform
		if (transform.localUpdateFlag)
		{
			transform.UpdateLocal();
			transform.UpdateWorld();	// flags with world update flag
		}
	}

	for (auto [entity, transform, hierarchy] : groupSceneGraph.each())
	{
		// if entity updated locally update local and world transform
		if (transform.localUpdateFlag)
		{
			transform.UpdateLocal();
			transform.UpdateWorld(try_get<Transform>(hierarchy.getParent()));
		}

		// if entity not updated locally and parent has transform
		else if (auto parentTransform = try_get<Transform>(hierarchy.getParent())) 
		{
			// if parent updated globally update, child of parent - entity
			if (parentTransform->worldUpdateFlag)
				transform.UpdateWorld(parentTransform);
		}
	}
}
