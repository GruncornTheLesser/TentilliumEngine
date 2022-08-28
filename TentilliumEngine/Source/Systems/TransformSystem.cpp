#include "TransformSystem.h"
#include <iostream>

void TransformSystem::update() {
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
	// in order of hierarchy
	for (auto [entity, hierarchy, transform] : viewTransform.each())
	{
		Transform* transformParent = try_get<Transform>(hierarchy.parent);	// parent transform if exists

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