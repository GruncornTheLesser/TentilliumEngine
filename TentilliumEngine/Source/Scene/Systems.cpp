#include "../Scene.h"

void Scene::HierarchyUpdate()
{
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

		std::cout << (int)entity << " - " << hierarchy.depth << std::endl;
	}

	// simplest solution -> doesnt necessarily work
	groupSceneGraph.sort<Hierarchy>([](const auto& lhs, const auto& rhs) { return lhs.depth < rhs.depth; });
}

void Scene::TransformUpdate()
{
	for (auto [entity, transform] : viewRootTransform.each())
	{
		// if entity's local transform changed, update local and world transform
		if (transform.localUpdateFlag)
		{
			transform.localMatrix = glm::translate(glm::mat4(1.0f), transform.position);
			transform.localMatrix = glm::scale(transform.localMatrix, transform.scale);
			transform.localMatrix *= glm::mat4(transform.rotation);

			transform.worldMatrix = transform.localMatrix;
			transform.worldUpdateFlag.Raise();
		}
	}

	for (auto [entity, transform, hierarchy] : groupSceneGraph.each())
	{
		auto parentTransform = try_get<Transform>(hierarchy.getParent());

		// if entity's local transform changed, update world and local transform
		if (transform.localUpdateFlag)
		{
			// update local transform
			transform.localMatrix = glm::translate(glm::mat4(1.0f), transform.position);
			transform.localMatrix = glm::scale(transform.localMatrix, transform.scale);
			transform.localMatrix *= glm::mat4(transform.rotation);

			if (parentTransform)
				transform.worldMatrix = parentTransform->worldMatrix * transform.localMatrix;
			else
				transform.worldMatrix = transform.localMatrix;

			transform.worldUpdateFlag.Raise();
			continue;
		}
		// if parent's world transform updated, update entity's world
		else if (parentTransform && parentTransform->worldUpdateFlag)
		{
			transform.worldMatrix = parentTransform->worldMatrix * transform.localMatrix;

			transform.worldUpdateFlag.Raise();
			continue;
		}
	}
}

void Scene::Render(const Shader& shader)
{
	for (auto [entity, render, transform] : viewRenderTransform.each())
	{
		glm::mat4 mvp = (glm::mat4)camera * (glm::mat4)transform;
		shader.setUniformMatrix4f("MVP", mvp);	// set uniform

		shader.bind();							// use shader program
		render.draw();
	}
}

void Scene::Testing(float time)
{
	// update transform

	for (auto [entity, transform] : viewTransform.each()) {
		//transform.setScale(glm::vec3(0.5f) * sin(0.3f * time));
		//transform.setPosition(glm::vec3(0, 0, 1) * sin(0.3f * time));
		transform.setRotation(glm::quat(glm::vec3(0.5f * sin(0.3f * time), time, 0)));
	}


}