#include "../Scene.h"
#include <gtx/transform.hpp>


void Scene::HierarchyUpdate()
{
	// i can iterate backwards, but I cant remove anything when I do.
	// this doesnt help me really because things are still added to the front of this
	// roo needs to be opposite end of newly added to maintain behaviour
	// 
	//root			        new added
	//	v					    v
	// [rbegin ..., ..., ..., rend]	
	// iteration -->				// can iterate but cant remove components
	//				  <-- iteration	// can iterate and remove components but wrong order for hierarchy

	std::vector<entt::entity> deferredDestroy;
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
		if (transform.localUpdateFlag) {
			transform.localMatrix =
				glm::translate(transform.position) *
				glm::mat4(transform.rotation) *
				glm::scale(transform.scale);

			transform.worldMatrix = transform.localMatrix;
			transform.worldUpdateFlag.Raise();
		}
	}

	for (auto it = groupSceneGraph.rbegin(); it != groupSceneGraph.rend(); it++) // hierarchy dependancy
	{
		auto [transform, hierarchy] = get<Transform, Hierarchy>(*it);
		auto parentTransform = try_get<Transform>(hierarchy.getParent());

		// if entity's local transform changed, update world and local transform
		if (transform.localUpdateFlag) {
			transform.localMatrix = 
				glm::translate(transform.position) * 
				glm::mat4(transform.rotation) * 
				glm::scale(transform.scale);
			
			if (parentTransform) transform.worldMatrix = parentTransform->worldMatrix * transform.localMatrix;
			else				 transform.worldMatrix = transform.localMatrix;

			transform.worldUpdateFlag.Raise();
			continue;
		}
		// if parent's world transform updated, update entity's world
		else if (parentTransform && parentTransform->worldUpdateFlag)
		{
			transform.worldMatrix = parentTransform->worldMatrix * transform.localMatrix;
			transform.worldUpdateFlag.Raise();
		}
	}
}

void Scene::Render(const Shader& shader)
{
	auto proj = (glm::mat4)get<Camera>(cam_entity);
	auto view = glm::inverse((glm::mat4)get<Transform>(cam_entity));

	for (auto [entity, render, transform] : viewRenderTransform.each()) {
		auto model = transform;
		glm::mat4 mvp = view * transform.worldMatrix;
		mvp = proj * mvp;
		shader.setUniformMatrix4f("MVP", mvp);
		shader.bind();
		render.draw();
	}
}

void Scene::Testing(float delta, float time)
{
	//get<Transform>((entt::entity)1).setPosition(sin(time), 0, 0);
	//get<Transform>((entt::entity)1).setRotation(sin(time), cos(time), 0);

}