#include "Scene.h"

#include <iostream>
#include <glm.hpp>
#include <stack>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Components/FlagManager.h"
#include "Rendering/Resources/Mesh.h"
#include "Rendering/Resources/Texture.h"

Scene::Scene() : camera(2.0f, 4.0f / 3.0f, 0.1f, 100.0f) { }

#define EMBEDDEDFILE(dir, res, ptr) (dir + "/" + #res + "/" + std::to_string((size_t)(ptr))).c_str()
entt::entity Scene::Load(std::string filepath)
{
	std::string dir = filepath.substr(0, filepath.find_first_of('.'));
	
	Assimp::Importer imp;
	const aiScene* scene = imp.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "ERROR::ASSIMP::" << imp.GetErrorString() << std::endl;
		throw std::exception();
	}

	std::map<aiNode*, entt::entity> entt_lookup;
	std::stack<aiNode*> sceneStack;
	sceneStack.push(scene->mRootNode);
	do {
		
		aiNode* node = sceneStack.top();
		sceneStack.pop();
		
		// create entity
		entt::entity e = create(); 
		entt_lookup.emplace(node, e);

		if (node->mParent) 
			emplace<Hierarchy>(e, entt_lookup[node->mParent]);

		emplace<Transform>(e, *(glm::mat4*)(&(node->mTransformation)));	// may need swizzling but i think its fine

		if (node->mNumMeshes > 0)
		{
			std::vector<std::shared_ptr<Mesh>> meshes;
			for (int i = 0; i < node->mNumMeshes; i++)
			{
				auto aiMsh = *(scene->mMeshes);
				auto aiMat = scene->mMaterials[aiMsh->mMaterialIndex];
				auto mat = Material::get_emplace(EMBEDDEDFILE(dir, material, aiMat), aiMat);
				meshes.push_back(Mesh::get_emplace(EMBEDDEDFILE(dir, texture, aiMsh), aiMsh, mat));
			}
			emplace<Model>(e, meshes);
		}
		
		
			
		//emplace<Light>(e, );

		// appropriate components add hierarchy, transform, animation, light, Mesh/Model, bone

		for (int i = 0; i < node->mNumChildren; i++)
			sceneStack.push(node->mChildren[i]);

	} while (!sceneStack.empty()); 

	/*
	if (scene->HasAnimations())
		emplace<AnimationHandler>(entt_lookup[scene->mRootNode], scene->mAnimations, scene->mNumAnimations);
	*/
	return entt_lookup[scene->mRootNode]; // return scene root
}









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
	for (auto [entity, model, transform] : viewRenderer.each())
	{
		glm::mat4 mvp = (glm::mat4)camera * (glm::mat4)transform;
		shader.setUniformMatrix4f("MVP", mvp);	// set uniform

		shader.bind();							// use shader program
		model.draw();
	}
}

void Scene::Testing(float time)
{
	// update transform

	for (auto [entity, transform] : viewTransform.each()) {
		transform.setRotation(glm::quat(glm::vec3(0.5f * sin(0.3f * time), time, 0)));
	}

	
}