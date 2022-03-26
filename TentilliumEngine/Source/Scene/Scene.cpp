#include "Scene.h"

#include "../Rendering/Resources/Mesh.h"
#include "../Rendering/Resources/Texture.h"

#include <queue>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>
#include <glew.h>
#include <glfw3.h>

#define EMBEDDEDFILE(dir, id) (dir + "/" + id).c_str()

// hierarchy		*DONE*
// transform		*DONE*
// model			*DONE*
// mesh				*DONE*
// material			*DONE*
// animation		
// lightEmitter		
// shadowCastor		
// Bone
// Skinning




entt::entity Scene::load(std::string filepath)
{
	Assimp::Importer imp;
	const aiScene* scene = imp.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GlobalScale | aiProcess_EmbedTextures);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "[Loading Error] : " << imp.GetErrorString() << std::endl;
		throw std::exception();
	}
	
	// array of embedded textures
	std::vector<std::shared_ptr<Texture>> textures;
	auto texPtr = scene->mTextures; 
	for (int i = 0; i < scene->mNumTextures; i++) {
		textures.push_back(Texture::get_or_default((filepath + "/*" + std::to_string(i)).c_str(), 
			texPtr[i]->mWidth, texPtr[i]->mHeight, NULL, texPtr[i]->pcData, Texture::FormatHint::RGBA));
	}

	std::map<aiNode*, entt::entity> entt_lookup;
	std::queue<aiNode*> sceneStack;
	sceneStack.push(scene->mRootNode);
	do 
	{
		// get next node
		aiNode* node = sceneStack.front();
		sceneStack.pop();

		// create entity
		entt::entity e;
		entt_lookup.emplace(node, e = create());


		// hierarchy component
		if (node->mParent)
		{
			auto x = entt_lookup[node->mParent];
			emplace<Hierarchy>(e, x);
		}
		
		// transform component
		auto transform = (aiMatrix4x4t<float>)(node->mTransformation);
		emplace<Transform>(e, *(glm::mat4*)&transform);
		
		// model/mesh/material components
		if (node->mNumMeshes > 0)
		{
			std::vector<Material> materials;
			std::vector<std::shared_ptr<Mesh>> meshes;
			
			for (int i = 0; i < node->mNumMeshes; i++)
			{
				auto aiMeshPtr = scene->mMeshes[node->mMeshes[i]];
				auto aiMatPtr = scene->mMaterials[aiMeshPtr->mMaterialIndex];
				
				meshes.push_back(Mesh::get_or_default(filepath + "/" + aiMeshPtr->mName.C_Str(), aiMeshPtr));
					
				Material& mat = materials.emplace_back();
					
				if (aiMatPtr->GetTextureCount(aiTextureType_DIFFUSE) > 0)
				{
					aiString texPath;
					aiMatPtr->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);
					mat.m_diff_map = Texture::load(filepath + "/" + texPath.data);
				}
				// else if colour value

				if (aiMatPtr->GetTextureCount(aiTextureType_NORMALS) > 0)
				{
					aiString texPath;
					aiMatPtr->GetTexture(aiTextureType_NORMALS, 0, &texPath);
					mat.m_norm_map = Texture::load(filepath + "/" + texPath.data);
				}

				if (aiMatPtr->GetTextureCount(aiTextureType_SPECULAR) > 0)
				{
					aiString texPath;
					aiMatPtr->GetTexture(aiTextureType_SPECULAR, 0, &texPath);
					mat.m_spec_map = Texture::load(filepath + "/" + texPath.data);
				}

				if (aiMatPtr->GetTextureCount(aiTextureType_SHININESS) > 0)
				{
					aiString texPath;
					aiMatPtr->GetTexture(aiTextureType_SHININESS, 0, &texPath);
					mat.m_shin_map = Texture::load(filepath + "/" + texPath.data);
				}
			}
			
			emplace<Model>(e, meshes, materials);
		}

		// traverse scene graph stack
		for (int i = 0; i < node->mNumChildren; i++)
			sceneStack.push(node->mChildren[i]);

	} while (!sceneStack.empty());

	/*
	if (scene->HasAnimations())
		emplace<AnimationHandler>(entt_lookup[scene->mRootNode], scene->mAnimations, scene->mNumAnimations);
	*/

	return entt_lookup[scene->mRootNode]; // return scene root
}

Scene::Scene() {
	defaultProgram = ShaderProgram::load("Resources/shaders/default.shader");
}
