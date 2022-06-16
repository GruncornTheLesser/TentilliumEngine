#include "TransformSystem.h"

// the Hierarchy is sorted in order of depth in reverse. The reverse order ensures that
// new hierarchies are caclulated after the originals. In a standard array it wouldnt be a 
// reverse its just the way entt choose to do it to prevent invalidating iterators.
//root			        new added
//	v					    v
// [rbegin ..., ..., ..., rend]	
// reverse iteration -->		// can iterate but cant remove components
//				  <-- iteration	// can iterate and remove components but wrong order for hierarchy
// the only system required for the hierarchy is to erase hierarchies with expired parents

// the Transform hierarchy updates the local and world matrices first from the root to the leaves.
// the roots are updated first using the viewRootTransform setting updating their world if the local 
// transform has changed. The hierarchied Transform components update the local transform and the 
// world transform from. This is done with 2 flags:
//		localUpdateFlag - raises whenever a value of the transform has been changed
//		worldUpdateFlag - raises whenever the world matrix has been changed
// 
// when iterating down the hierarchies, the parents are always updated first so the flags can determine
// the behaviour of the system.
// if localUpdateFlag -> update local and world transform 
// if worldUpdateFlag -> update world transform only
// if entity has parent -> world transform updates from the parent.
// else					-> updates direclty from local transform
// 
// because the transform and hierarchy are completely decoupled(ie one can exist without the other)
// the parent could exist without a transform. In this case the transform treats the component as a root.
void TransformSystem::update() {
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
}