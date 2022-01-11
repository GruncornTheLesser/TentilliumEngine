#pragma once
#include<vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


struct mesh
{

};

struct material
{

};

/*
void loadModel(const char* path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "Assimp Error - " << import.GetErrorString() << std::endl;
		throw std::exception();
	}

	processNode(scene->mRootNode, scene);
}
void processNode(aiNode* node, const aiScene* scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}
Mesh processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// process vertex positions, normals and texture coordinates
		[...]
		vertices.push_back(vertex);
	}
	// process indices
	[...]
	// process material
	if (mesh->mMaterialIndex >= 0)
	{
		[...]
	}

	return Mesh(vertices, indices, textures);
}
*/
class ModelComponent
{

public:
	ModelComponent();
	ModelComponent(const char* filename);
	~ModelComponent();

	void Draw();
};

