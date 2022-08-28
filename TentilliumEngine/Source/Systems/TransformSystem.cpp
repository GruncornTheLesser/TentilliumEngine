#include "TransformSystem.h"
#include <iostream>

void TransformSystem::update() {
	// Transform System
	for (auto [entity, transform] : viewRootTransform.each())
	{
		// if entity's local transform changed, update local and world transform
		if (transform.m_updateFlag) {
			transform.updateLocal();							// update local matrix from position, rotation and scale
			transform.m_worldMatrix = transform.m_localMatrix;	// update world matrix from local matrix
		}
	}
	// in order of hierarchy
	for (auto [entity, hierarchy, transform] : viewTransform.each())
	{
		Transform* transformParent = try_get<Transform>(hierarchy.parent);	// parent transform if exists

		if ((transformParent && transformParent->m_updateFlag))		// if parent's world transform updated
		{															// update entity's world transform only
			
			if (transform.m_updateFlag)				// if parent has no transform and entity's local transform changed
				transform.updateLocal();			// update local matrix from position, scale and rotation
			else 
				transform.m_updateFlag.raise();		// raise update flag as world transform changed

			transform.m_worldMatrix = transformParent->m_worldMatrix * transform.m_localMatrix;
			
		}
		else if (transform.m_updateFlag)	// if parent has no transform and entity's local transform changed
		{									// update entity's local transform then set world transform to local
			transform.updateLocal();
			transform.m_worldMatrix = transform.m_localMatrix;
		}
	}
}