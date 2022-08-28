#include "TransformSystem.h"
#include <iostream>

void TransformSystem::update() {
	/*
	for (auto [entity, transform] : viewRootTransform.each())
	{
		// if entity's local transform changed, update local and world transform
		if (transform.m_localUpdateFlag) {
			transform.updateLocal();							// update local matrix from position, rotation and scale
			transform.m_worldMatrix = transform.m_localMatrix;	// update world matrix from local matrix
			transform.m_worldUpdateFlag.Raise();				// flag transform as having updated world
		}
	}

	std::list<entt::entity> deferredDestroy;
	for (auto it = viewHierarchy.rbegin(); it != viewHierarchy.rend(); it++)
	{
		Hierarchy& hierarchy = get<Hierarchy>(*it);					// get hierarchy
		Transform* transform = try_get<Transform>(*it);				// get transform if exists
		Transform* parent = nullptr;								// parent transform if exists

		if (valid(hierarchy.parent))								// if hierarchy valid
		{
			if (transform)											// if entity has transform
				parent = try_get<Transform>(hierarchy.getParent());	// get parent transform
			else continue;											// skip the transform system
		}
		else
		{
			deferredDestroy.push_back(*it);							// erase invalid hierarchy
			if (!transform)											// if entity does not have transform
				continue;											// skip the transform system
		}

		if (parent && transform->m_localUpdateFlag)					// if local transform changed
		{															// update entity's local and world transform 
			transform->updateLocal();
			transform->m_worldMatrix = parent->m_worldMatrix * transform->m_localMatrix;
			transform->m_worldUpdateFlag.Raise();
		}
		else if (parent && parent->m_worldUpdateFlag)				// if parent's world transform updated
		{															// update entity's world transform only
			transform->m_worldMatrix = parent->m_worldMatrix * transform->m_localMatrix;
			transform->m_worldUpdateFlag.Raise();
		}
		else if (!parent && transform->m_localUpdateFlag)			// if parent has no transform and entity's local transform changed
		{															// update entity's local transform then set world transform to local
			transform->updateLocal();
			transform->m_worldMatrix = transform->m_localMatrix;	// this is necessary because transform and hierarchy are decoupled
			transform->m_worldUpdateFlag.Raise();
		}
	}

	erase<Hierarchy>(deferredDestroy.begin(), deferredDestroy.end());
	*/

	// Transform System
	for (auto [entity, transform] : viewRootTransform.each())
	{
		// if entity's local transform changed, update local and world transform
		if (transform.m_localUpdateFlag) {
			transform.updateLocal();							// update local matrix from position, rotation and scale
			transform.m_worldMatrix = transform.m_localMatrix;	// update world matrix from local matrix
			transform.m_worldUpdateFlag.raise();				// flag transform as having updated world
		}
	}
	
	for (auto [entity, hierarchy, transform] : viewTransform.each()) // needs to be reversed
	{
		Transform* transformParent = try_get<Transform>(hierarchy.m_parent);	// parent transform if exists

		if ((transformParent && transformParent->m_worldUpdateFlag))		// or parent changed
		{																	// update entity's world transform only
			if (transform.m_localUpdateFlag)								// if parent has no transform and entity's local transform changed
				transform.updateLocal();

			transform.m_worldMatrix = transformParent->m_worldMatrix * transform.m_localMatrix;
			transform.m_worldUpdateFlag.raise();
		}
		else if (transform.m_localUpdateFlag)					// if parent has no transform and entity's local transform changed
		{														// update entity's local transform then set world transform to local
			transform.updateLocal();
			transform.m_worldMatrix = transform.m_localMatrix;	// this is necessary because transform and hierarchy are decoupled
			transform.m_worldUpdateFlag.raise();
		}
	}
}