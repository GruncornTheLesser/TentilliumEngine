#include "Scene.h"
#include "Components/FlagManager.h"
#include <iostream>
#include <glm.hpp>
//#include <gtc/matrix_transform.hpp>

void Scene::HierarchyUpdate()
{
	// use cases:
	// 
	// user removes parent												// hierarchy must be removed
	// user adds hierarchy with parent to pre-existing hierarchy		// must be added to calculate after parent and is
	// user adds hierarchy with parent to not yet initiated hierarchy	// must be added to calculate after parent -> fucks up
	// user adds hierarchy with parent to entity						// no dependency
	// user sets hierarchy parent to more deep branch					// fucks up order -> entity order needs to be changed
	// user sets hierarchy parent to less deep branch					// maintains correct order
	// user removes hierarchy											// no problems

	// compare entity.depth to parent.depth + 1
	// if the depth is equal, do nothing
	// if the depth is greater, flag depth update
	// if the depth is less, push to back of group and flag depth update
	// set entity.depth = parent.depth + 1
	
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

	groupSceneGraph.sort<Hierarchy>([](const auto& lhs, const auto& rhs) { return lhs.depth < rhs.depth; });
}

void Scene::TransformUpdate()
{
	// search for worldupdateflags transforms
	for (auto [entity, transform, hierarchy] : groupSceneGraph.each())
	{
		if (auto parentTransform = try_get<Transform>(hierarchy.getParent()))
		{
			if ((parentTransform->localUpdateFlag) || (parentTransform->worldUpdateFlag)) {
				transform.worldUpdateFlag.Raise();
			}
		}
	}

	for (auto [entity, transform] : viewRootTransform.each())
	{
		if (transform.localUpdateFlag)
			transform.UpdateLocal();
		
		if (transform.worldUpdateFlag)
			transform.UpdateWorld();
	}

	for (auto [entity, transform, hierarchy] : groupSceneGraph.each())
	{
		if (transform.localUpdateFlag)
			transform.UpdateLocal();
		
		if (transform.worldUpdateFlag)
			transform.UpdateWorld(try_get<Transform>(hierarchy.getParent()));
	}
}
