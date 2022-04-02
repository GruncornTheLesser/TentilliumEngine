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

#define MESH_FILENAME(filepath, node, mesh) filepath + "/" + node->mName.C_Str() + "/" + mesh->mName.C_Str()
#define MAT_FILENAME(filepath, node, mat) filepath + "/" + node->mName.C_Str() + "/" + mat->GetName().C_Str()

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

std::shared_ptr<Mesh> loadMesh(aiMesh* aiMeshPtr, std::string filename)
{
	if (auto mesh = Mesh::get(filename))
		return mesh;

	size_t index_size = aiMeshPtr->mNumFaces * 3;
	size_t vertex_size = aiMeshPtr->mNumVertices;
	
	unsigned int* index_data	= new unsigned int[index_size];
	float* position_data		= nullptr;
	float* tCoord_data			= nullptr;
	float* normal_data			= nullptr;
	float* colour_data			= nullptr;

	if (index_data) {
		for (unsigned int i = 0; i < aiMeshPtr->mNumFaces; i++)
		{
			index_data[i * 3 + 0] = aiMeshPtr->mFaces[i].mIndices[0];
			index_data[i * 3 + 1] = aiMeshPtr->mFaces[i].mIndices[1];
			index_data[i * 3 + 2] = aiMeshPtr->mFaces[i].mIndices[2];
		}
	}
	
	if (aiMeshPtr->HasPositions()) {
		position_data = (float*)aiMeshPtr->mVertices;
	}
	
	if (aiMeshPtr->HasTextureCoords(0)) 
	{	// texels are annoying becuase there are 3 of them in assimp specification
		tCoord_data = new float[aiMeshPtr->mNumVertices * 2u];
		for (unsigned int i = 0; i < aiMeshPtr->mNumVertices; i++)
		{
			tCoord_data[i * 2 + 0] = aiMeshPtr->mTextureCoords[0][i].x;
			tCoord_data[i * 2 + 1] = aiMeshPtr->mTextureCoords[0][i].y;
		}
	}

	if (aiMeshPtr->HasNormals()) {
		normal_data = (float*)aiMeshPtr->mNormals;
	}
	
	if (aiMeshPtr->HasVertexColors(0))
	{
		colour_data = new float[aiMeshPtr->mNumVertices * 4u];
		for (unsigned int i = 0; i < aiMeshPtr->mNumVertices; i++)
		{
			colour_data[i * 2 + 0] = aiMeshPtr->mColors[0][i].r;
			colour_data[i * 2 + 1] = aiMeshPtr->mColors[0][i].g;
			colour_data[i * 2 + 2] = aiMeshPtr->mColors[0][i].b;
			colour_data[i * 2 + 3] = aiMeshPtr->mColors[0][i].a;
		}
	}

	auto mesh = Mesh::stash(filename, index_size * sizeof(unsigned int), vertex_size * sizeof(float), 
		index_data, position_data, tCoord_data, normal_data, colour_data);

	// only arrays created by this function are deleted
	// other arrays managed from elsewhere
	if (index_data) delete[] index_data;
	if (tCoord_data) delete[] tCoord_data;
	if (colour_data) delete[] colour_data;

	return mesh;
}

std::shared_ptr<Material> loadMaterial(aiMaterial* aiMatPtr, std::string filename) {

	if (auto mat = Material::get(filename))
		return mat;

	Material mat = Material();

	aiColor3D colour;
	aiString texture;
	
	if (aiMatPtr->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), texture) == AI_SUCCESS)
		mat.Diffuse_map = Texture::get(texture.C_Str());
	else if (aiMatPtr->Get(AI_MATKEY_COLOR_DIFFUSE, colour) == AI_SUCCESS)
		mat.Diffuse_val = glm::vec3(colour.r, colour.g, colour.b);

	if (aiMatPtr->Get(AI_MATKEY_TEXTURE_SPECULAR(0), texture) == AI_SUCCESS)
		mat.Specular_map = Texture::get(texture.C_Str());
	else if (aiMatPtr->Get(AI_MATKEY_COLOR_SPECULAR, colour) == AI_SUCCESS)
		mat.Specular_val = colour.r;

	if (aiMatPtr->Get(AI_MATKEY_TEXTURE_SHININESS(0), texture) == AI_SUCCESS)
		mat.Gloss_map = Texture::get(texture.C_Str());
	else if (aiMatPtr->Get(AI_MATKEY_SHININESS, colour) == AI_SUCCESS)
		mat.Gloss_val = colour.r;
	
	return Material::stash(filename, std::move(mat));
}



entt::entity Scene::load(std::string filepath)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs
		| aiProcess_GlobalScale | aiProcess_EmbedTextures);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "[Loading Error] : " << importer.GetErrorString() << std::endl;
		throw std::exception();
	}
	
	// array of embedded textures
	std::vector<std::shared_ptr<Texture>> textures;
	if (scene->HasTextures()) {
		auto texPtr = scene->mTextures; 
		for (unsigned int i = 0; i < scene->mNumTextures; i++) {
			textures.push_back(Texture::get_or_default(texPtr[i]->mFilename.C_Str(),
				texPtr[i]->mWidth, texPtr[i]->mHeight, NULL, texPtr[i]->pcData, Texture::Format::RGBA));
		}
	}
	
	std::map<aiNode*, entt::entity> entt_lookup;
	std::queue<aiNode*> sceneStack;
	sceneStack.push(scene->mRootNode);
	do {
		// get next node
		aiNode* node = sceneStack.front();
		sceneStack.pop();

		// create entity
		entt::entity e = create();
		entt_lookup.emplace(node, e);

		// queue children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
			sceneStack.push(node->mChildren[i]);

		// hierarchy component
		if (node->mParent) {
			auto x = entt_lookup[node->mParent];
			emplace<Hierarchy>(e, x);
		}
		
		// transform component
		auto transform = (aiMatrix4x4t<float>)(node->mTransformation);
		emplace<Transform>(e, *(glm::mat4*)&transform);
		
		// model mesh,material,texture components
		if (node->mNumMeshes > 0)
		{
			auto& model = emplace<Model>(e);

			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				auto aiMeshPtr = scene->mMeshes[node->mMeshes[i]];
				auto aiMatPtr = scene->mMaterials[aiMeshPtr->mMaterialIndex];

				model.add(
					loadMesh(aiMeshPtr, MESH_FILENAME(filepath, node, aiMeshPtr)),
					loadMaterial(aiMatPtr, MAT_FILENAME(filepath, node, aiMatPtr)));
			}
		}

	} while (!sceneStack.empty());

	/*
	if (scene->HasAnimations())
		emplace<AnimationHandler>(entt_lookup[scene->mRootNode], scene->mAnimations, scene->mNumAnimations);
	*/

	return entt_lookup[scene->mRootNode]; // return scene root
}

