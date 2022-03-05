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

#define ResourceFile(dir, res, ptr) dir + "/" + #res + "/" + std::to_string((size_t)(ptr)) + (std::string(#res)).substr(0, 3)

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

	std::vector<std::shared_ptr<Texture>> textures;

	// meshes, materials, textures, animations, 
	if (scene->HasTextures())
	{
		aiTexture** it = scene->mTextures;
		for (int i = 0; i < scene->mNumTextures; i++)
		{
			/*ResourceFile(dir, textures, *it)*/
			emplace<std::shared_ptr<Texture>>(create(), Texture::stash(std::string("texture"), new Texture(*it)));
		}
	}
	
	if (scene->HasMaterials())
	{
		aiMaterial** it = scene->mMaterials;
		for (int i = 0; i < scene->mNumMaterials; i++)
			std::cout << "materials have not been implemented yet\n";
			//Material::stash(ResourceFile(dir, materials, *it), new Material(*it++));
	}
	
	if (scene->HasAnimations())
	{
		aiAnimation** it = scene->mAnimations;
		for (int i = 0; i < scene->mNumAnimations; i++)
			std::cout << "animations have not been implemented yet\n";
	}
	
	if (scene->HasLights())
	{
		aiLight** it = scene->mLights;
		for (int i = 0; i < scene->mNumAnimations; i++)
			std::cout << "lights have not been implemented yet\n"; // god on day 3
	}
	

	std::stack<aiNode*> sceneStack;
	sceneStack.push(scene->mRootNode);
	do {// depth first iteration
		aiNode* node = sceneStack.top();
		sceneStack.pop();

		//auto e = CreateEntity();
		//emplace<Transform>(e, node->mTransformation);
		//if (node->mParent)
		//	emplace<Hierarchy>(e, e); // get parent entity

		for (int i = 0; i < node->mNumChildren; i++)
			sceneStack.push(node->mChildren[i]);

	} while (!sceneStack.empty()); 

	return (entt::entity)0;
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
	auto transform = try_get<Transform>((entt::entity)0);
	transform->setRotation(glm::quat(glm::vec3(0.5f * sin(0.3f * time), time, 0)));
}