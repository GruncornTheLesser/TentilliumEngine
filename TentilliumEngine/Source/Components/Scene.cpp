#include "Scene.h"
#include <iostream>

void Scene::TagUpdate()
{
	clear<transform_update_local_tag>();
	clear<transform_update_world_tag>();
	clear<hierarchy_update_depth_tag>();

	// could be replaced by an observer
	//			-> ugly
	//			-> order gets broken
	//			-> requires patch function call
	
	// flat function call in scene setPosition(entity, pos) { adds update tag to entity sets position }
	//			-> no garentee that the function call on the object has a transform component

	// store entity id and registry ptr in component???
	//			-> possibly cant store entity -> value might change?? i dont think it will but theres a line in the docs that says i shouldnt 
	//			-> moronic -> stores uneccessary data
	//			-> still needs reference to the registry that it belongs to

	for (auto [entity, transform] : viewTransform.each())
		if (transform.version == Transform::currentVersion)
		{
			emplace<transform_update_local_tag>(entity);
			emplace<transform_update_world_tag>(entity);
		}
	
	Transform::currentVersion++;

	// hierarchy dirty propagation
	for (auto [entity, hierarchy] : viewHierarchy.each())
	{
		if (valid(hierarchy.parent))
		{
			if (any_of<hierarchy_update_depth_tag>(hierarchy.parent))
				emplace<hierarchy_update_depth_tag>(entity);

			if (any_of<transform_update_world_tag, transform_update_local_tag>(hierarchy.parent))
				emplace<transform_update_world_tag>(entity);
		}
		else
		{
			erase<Hierarchy>(entity);
			emplace<hierarchy_update_depth_tag>(entity);
			emplace<transform_update_world_tag>(entity);
		}
	}
}

void Scene::Calculate()
{
	for (auto [entity, transform] : viewLocalTransformUpdate.each())
	{
		transform.localMatrix = glm::translate(mat4(1.0f), transform.position);
		transform.localMatrix = glm::scale(transform.localMatrix, transform.scale);
		transform.localMatrix *= mat4(transform.rotation);
		
		std::cout << transform.localMatrix[3][0] << ", " << transform.localMatrix[3][1] << ", " << transform.localMatrix[3][2] << std::endl;
	}

	for (auto [entity, transform] : viewRootTransform.each())
	{
		transform.worldMatrix = transform.localMatrix;
		std::cout << transform.worldMatrix[3][0] << ", " << transform.worldMatrix[3][1] << ", " << transform.worldMatrix[3][2] << std::endl;
	}

	for (auto [entity, transform, hierarchy] : viewBranchTransform.each())
	{
		if (auto parent = try_get<Transform>(hierarchy.getParent()))
			transform.worldMatrix = parent->worldMatrix * transform.localMatrix;
		else
			transform.worldMatrix = transform.localMatrix;

		std::cout << transform.worldMatrix[3][0] << ", " << transform.worldMatrix[3][1] << ", " << transform.worldMatrix[3][2] << std::endl;
	}
}
