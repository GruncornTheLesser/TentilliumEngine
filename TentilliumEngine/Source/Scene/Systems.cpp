#include "../Scene.h"
#include <gtx/transform.hpp>
#include <list>

void Scene::HierarchyUpdate()
{
	// the Hierarchy system is sorted in order of depth in reverse. The reverse order ensures that
	// new hierarchies are caclulated after the originals. In a standard array it wouldnt be a 
	// reverse its just the way entt choose to do it to prevent invalidating iterators.
	//root			        new added
	//	v					    v
	// [rbegin ..., ..., ..., rend]	
	// reverse iteration -->		// can iterate but cant remove components
	//				  <-- iteration	// can iterate and remove components but wrong order for hierarchy

	std::list<entt::entity> deferredDestroy;
	for (auto it = viewHierarchy.rbegin(); it != viewHierarchy.rend(); it++) // hierarchy dependancy
	{
		auto hierarchy = &viewHierarchy.get<Hierarchy>(*it);

		if (!valid(hierarchy->parent)) {
			hierarchy->depth = 0;
			deferredDestroy.push_back(*it);
		}

		hierarchy->depth = 1;
		auto parent = try_get<Hierarchy>(hierarchy->parent);
		if (parent) 
			hierarchy->depth += parent->depth;
	}
	std::cout << deferredDestroy.size() << " hierarchies expired" << std::endl;
	erase<Hierarchy>(deferredDestroy.begin(), deferredDestroy.end());
}

void Scene::TransformUpdate()
{
	for (auto [entity, transform] : viewRootTransform.each()) // no hierarchy dependancy
	{
		// if entity's local transform changed, update local and world transform
		if (transform.m_localUpdateFlag) {
			transform.updateLocal();							// update local matrix from position, rotation and scale
			transform.m_worldMatrix = transform.m_localMatrix;	// update world matrix from local matrix
			transform.m_worldUpdateFlag.Raise();				// flag transform as having updated world
		}
	}

	for (auto it = groupSceneGraph.rbegin(); it != groupSceneGraph.rend(); it++) // hierarchy dependancy
	{
		auto [transform, hierarchy] = get<Transform, Hierarchy>(*it);			 // get components
		auto parent = try_get<Transform>(hierarchy.getParent());				// get parent in hierarchy component

		// if entity's local transform changed, update world and local transform
		if (transform.m_localUpdateFlag) {
			transform.updateLocal();	// update local matrix from position rotation and scale

			if (parent)
				transform.m_worldMatrix = parent->m_worldMatrix * transform.m_localMatrix;
			else				 
				transform.m_worldMatrix = transform.m_localMatrix;
			transform.m_worldUpdateFlag.Raise();
		}
			

		// if parent's world transform updated, update entity's world
		else if (parent && parent->m_worldUpdateFlag)
		{
			transform.m_worldMatrix = parent->m_worldMatrix * transform.m_localMatrix;
			transform.m_worldUpdateFlag.Raise();
		}
	}
}

void Scene::Render(const Shader& shader)
{
	auto proj = (glm::mat4)get<Camera>(cam_entity);
	auto view = glm::inverse((glm::mat4)get<Transform>(cam_entity));

	for (auto [entity, render, transform] : viewRenderTransform.each()) {
		glm::mat4 mvp = proj * view * transform.m_worldMatrix;
		shader.setUniformMatrix4f("MVP", mvp);
		shader.bind();
		render.draw();
	}
}