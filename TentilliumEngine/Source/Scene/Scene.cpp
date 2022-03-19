#include "../Scene.h"
#include "../Scene.h"

#include <queue>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Rendering/Resources/Mesh.h"
#include "../Rendering/Resources/Texture.h"

#define EMBEDDEDFILE(dir, res, name) (dir + "/" + #res + "/" + name).c_str()
entt::entity Scene::load(std::string filepath)
{
	std::string dir = filepath.substr(0, filepath.find_first_of('.'));

	Assimp::Importer imp;
	const aiScene* scene = imp.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "ERROR::ASSIMP::" << imp.GetErrorString() << std::endl;
		throw std::exception();
	}

	/*
	// for testing
	if (scene->mTextures)
		outTexture = Texture::stash((*scene->mTextures)->mFilename.C_Str(), *scene->mTextures);
	else
		outTexture = Texture::load("Resources/images/Image4.png");
		*/
	
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

		if (node->mParent)
		{
			auto x = entt_lookup[node->mParent];
			emplace<Hierarchy>(e, x);
		}
			
		//aiMatrix4x4t<float> x = (aiMatrix4x4t<float>)(node->mTransformation);
		auto transform = (aiMatrix4x4t<float>)(node->mTransformation);
		emplace<Transform>(e, *(glm::mat4*)&transform);
		
		if (node->mNumMeshes > 0)
		{
			std::vector<std::shared_ptr<Mesh>> meshes;
			for (int i = 0; i < node->mNumMeshes; i++)
			{
				auto aiMsh = scene->mMeshes[node->mMeshes[i]];
				auto aiMat = scene->mMaterials[aiMsh->mMaterialIndex];
				auto mat = Material::get_or_default(EMBEDDEDFILE(dir, material, aiMat->GetName().C_Str()), aiMat);
				meshes.push_back(Mesh::get_or_default(EMBEDDEDFILE(dir, mesh, node->mName.C_Str()), aiMsh, mat));
			}
			emplace<Model>(e, meshes);
		}


		// add hierarchy, transform, animation, light, Mesh/Model, bone

		for (int i = 0; i < node->mNumChildren; i++)
			sceneStack.push(node->mChildren[i]);

	} while (!sceneStack.empty());

	/*
	if (scene->HasAnimations())
		emplace<AnimationHandler>(entt_lookup[scene->mRootNode], scene->mAnimations, scene->mNumAnimations);
	*/

	HierarchyUpdate();
	return entt_lookup[scene->mRootNode]; // return scene root
}

void Scene::setCamera(entt::entity entity) {
	if (!valid(cam_entity) || !any_of(cam_entity)) return;
	cam_entity = entity;
}

entt::entity Scene::getCamera() {
	return cam_entity;
}