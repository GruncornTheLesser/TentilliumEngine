#include "LoadSystem.h"
#include "../Components/Mesh.h"
#include "../Components/Hierarchy.h"
#include "../Components/Transform.h"
#include "../Components/SpecularMaterial.h"
#include "../Rendering/Resources/Texture.h"
#include <queue>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>

// hierarchy		*DONE*
// transform		*DONE*
// mesh				*DONE*
// material			*DONE* ish
// animation
// lightEmitter
// shadowCastor
// Bones
// Skinning

//#define MESH_FILENAME(filepath, node, mesh) filepath + "/" + node->mName.C_Str() + "/" + mesh->mName.C_Str()
//#define MAT_FILENAME(filepath, node, mat) filepath + "/" + node->mName.C_Str() + "/" + mat->GetName().C_Str()

// TODO:
// optimize scene graph
//		-> not impressed by assimp graph optimization. 
//		-> automatically sort the useless nodes and attach them to parents to reduce the node spam
//		-> its kinda ridiculous
//		-> requires collection of parts and delayed assigning of components???
//		-> alternatively I could just assign the meshes to the parent entity

// add bounding box to Transform vao combo for screen space culling optimizations
//		-> Make Transform a requirement of Specular system???
//		

// how to solve multiple meshes per entity
//		combine meshes in to single vaos and vbos 
//			-> could adapt vao and material to distinguish meshes with indexes of the mesh
//			-> probably best to keep them seperate tho
//			-> requires manipulation of the raw data
//			-> requires changes to material to identify the mesh groups
//			changes to material:
//				1:n relationship for entity to material
//					-> complicated and not necessarily a performance improvement
//					-> better syntax
//				a vector in material to identify the seperations 
//					-> simple
//					-> nasty to set material properties
// 
//		make mesh a child entity with vbos, vao and components
//			-> simple
//			-> bad syntax
//			-> not very data oriented
//			-> doesnt have transform component so will get annoying 
// 
//		
//		
//			

// blender's vertex count is wrong -> it does not identify unique vertices properly
// if a 2 vertices have the same position but different normals it is incorrectly
// marked as unique.

// restructure to prioritise relevant nodes:
// root node + Assimilated meshes + transform

entt::entity LoadSystem::load(std::string filepath)
{
	Assimp::Importer importer;
	
	const aiScene* scene = importer.ReadFile(filepath, 
		aiProcess_Triangulate | 
		aiProcess_FlipUVs | 
		aiProcess_GlobalScale | 
		aiProcess_EmbedTextures |
		aiProcess_OptimizeGraph | // doesnt do enough
		aiProcess_OptimizeMeshes |
		aiProcess_JoinIdenticalVertices);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "[Loading Error] : " << importer.GetErrorString() << std::endl;
		throw std::exception();
	}

	// array of textures
	std::vector<std::shared_ptr<Texture>> textures;
	if (scene->HasTextures()) {
		auto texPtr = scene->mTextures;
		for (unsigned int i = 0; i < scene->mNumTextures; i++) {
			textures.push_back(Texture::get_or_default(texPtr[i]->mFilename.C_Str(),
				texPtr[i]->mWidth, texPtr[i]->mHeight, NULL, texPtr[i]->pcData, Texture::Format::RGBA));
		}
	}
	
	entt::entity e = create();

	if (scene->HasMeshes()) 
	{
		// get size needed for buffers
		unsigned int vertexCount = 0, faceCount = 0;
		for (unsigned int mesh_i = 0; mesh_i < scene->mNumMeshes; mesh_i++) {
			vertexCount += scene->mMeshes[mesh_i]->mNumVertices;
			faceCount += scene->mMeshes[mesh_i]->mNumFaces;
		}

		unsigned int vertexOffset = 0, faceOffset = 0, boneOffset = 0;
		for (unsigned int mesh_i = 0; mesh_i < scene->mNumMeshes; mesh_i++)
		{
			auto aiMeshPtr = scene->mMeshes[mesh_i];

			if (aiMeshPtr->HasFaces()) 
			{
				std::vector<unsigned int> indices;
				for (unsigned int face_i = 0; face_i < aiMeshPtr->mNumFaces; face_i++) 
				{
					auto& face = aiMeshPtr->mFaces[face_i];
					for (unsigned int index_i = 0; index_i < face.mNumIndices; index_i++)
						indices.push_back(face.mIndices[index_i] + vertexOffset);
				}
				get_or_emplace<VBO<Index>>(e, nullptr, sizeof(int) * faceCount * 3)
					.set_data(&indices[0], sizeof(int) * 3 * aiMeshPtr->mNumFaces, sizeof(int) * 3 * faceOffset);
			}

			if (aiMeshPtr->HasPositions()) {
				get_or_emplace<VBO<Position>>(e, nullptr, sizeof(float) * 3 * vertexCount)
					.set_data(aiMeshPtr->mVertices, sizeof(float) * 3 * aiMeshPtr->mNumVertices, sizeof(float) * 3 * vertexOffset);
			}

			if (aiMeshPtr->HasNormals()) {
				get_or_emplace<VBO<Normal>>(e, nullptr, sizeof(float) * 3 * vertexCount)
					.set_data(aiMeshPtr->mNormals, sizeof(float) * 3 * aiMeshPtr->mNumVertices, sizeof(float) * 3 * vertexOffset);
			}

			if (aiMeshPtr->HasTextureCoords(0)) {
				std::vector<float> texCoords;
				for (unsigned int uv_i = 0; uv_i < aiMeshPtr->mNumVertices; uv_i++) {
					texCoords.push_back(aiMeshPtr->mTextureCoords[0][uv_i].x);
					texCoords.push_back(aiMeshPtr->mTextureCoords[0][uv_i].y);
				}
				get_or_emplace<VBO<TexCoord>>(e, nullptr, sizeof(float) * vertexCount * 2)
					.set_data(&texCoords[0], sizeof(float) * aiMeshPtr->mNumVertices * 2, sizeof(float) * vertexOffset * 2);
			}

			vertexOffset += aiMeshPtr->mNumVertices;
			faceOffset += aiMeshPtr->mNumFaces;
		}

		// has bones
	}

	if (scene->HasMaterials()) {
		auto aiMatPtr = scene->mMaterials[0];

		auto& mat = emplace<SpecularMaterial>(e);

		aiColor3D colour;
		aiString texture;

		if (aiMatPtr->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), texture) == AI_SUCCESS)
			mat.set_diff(Texture::get(texture.C_Str()));
		else if (aiMatPtr->Get(AI_MATKEY_COLOR_DIFFUSE, colour) == AI_SUCCESS)
			mat.set_diff(glm::vec3(colour.r, colour.g, colour.b));

		if (aiMatPtr->Get(AI_MATKEY_TEXTURE_SPECULAR(0), texture) == AI_SUCCESS)
			mat.set_spec(Texture::get(texture.C_Str()));
		else if (aiMatPtr->Get(AI_MATKEY_COLOR_SPECULAR, colour) == AI_SUCCESS)
			mat.set_spec(colour.r);

		if (aiMatPtr->Get(AI_MATKEY_TEXTURE_SHININESS(0), texture) == AI_SUCCESS)
			mat.set_glos(Texture::get(texture.C_Str()));
		else if (aiMatPtr->Get(AI_MATKEY_SHININESS, colour) == AI_SUCCESS)
			mat.set_glos(colour.r);

	}

	/*
	std::map<aiNode*, entt::entity> entt_lookup;
	std::queue<aiNode*> sceneStack;
	sceneStack.push(scene->mRootNode);
	do {
		// get next node
		aiNode* node = sceneStack.front();
		sceneStack.pop();

		std::cout << node->mName.C_Str() << " ";

		// create entity
		entt::entity e = create();
		entt_lookup.emplace(node, e);

		// enqueue children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
			sceneStack.push(node->mChildren[i]);
		
		// hierarchy component
		if (node->mParent) {
			emplace<Hierarchy>(e, entt_lookup[node->mParent]);
			std::cout << "hierarchy, ";
		}

		// transform component -> if it has no transform it can more than likely be used as a component container
		auto transform = (aiMatrix4x4t<float>)(node->mTransformation);
		emplace<Transform>(e, *(glm::mat4*)&transform);

		// model mesh, material, texture components
		// combine all meshes in this entity into 1 mesh
		if (node->mNumMeshes > 0)
		{
			std::cout << "mesh, ";

			// get size needed for buffers
			unsigned int vertexCount = 0, faceCount = 0;
			for (unsigned int mesh_i = 0; mesh_i < node->mNumMeshes; mesh_i++) {
				vertexCount += scene->mMeshes[node->mMeshes[mesh_i]]->mNumVertices;
				faceCount += scene->mMeshes[node->mMeshes[mesh_i]]->mNumFaces;
			}

			// iterate through meshes and add to buffers
			unsigned int vertexOffset = 0, faceOffset = 0, boneOffset = 0;
			for (unsigned int mesh_i = 0; mesh_i < node->mNumMeshes; mesh_i++)
			{
				auto aiMeshPtr = scene->mMeshes[node->mMeshes[mesh_i]];
				
				if (aiMeshPtr->HasFaces()) {
					std::vector<unsigned int> indices;
					for (unsigned int face_i = 0; face_i < aiMeshPtr->mNumFaces; face_i++) {
						auto& face = aiMeshPtr->mFaces[face_i];
						for (unsigned int index_i = 0; index_i < face.mNumIndices; index_i++)
							indices.push_back(face.mIndices[index_i]);
					}
					get_or_emplace<VBO<Index>>(e, nullptr, sizeof(int) * faceCount * 3)
						.set_data(&indices[0], sizeof(int) * 3 * aiMeshPtr->mNumFaces, sizeof(int) * 3 * faceOffset);
				}

				if (aiMeshPtr->HasPositions()) {
					get_or_emplace<VBO<Position>>(e, nullptr, sizeof(float) * 3 * vertexCount)
						.set_data(aiMeshPtr->mVertices, sizeof(float) * 3 * aiMeshPtr->mNumVertices, sizeof(float) * 3 * vertexOffset);
				}

				if (aiMeshPtr->HasNormals()) {
					get_or_emplace<VBO<Normal>>(e, nullptr, sizeof(float) * 3 * vertexCount)
						.set_data(aiMeshPtr->mNormals, sizeof(float) * 3 * aiMeshPtr->mNumVertices, sizeof(float) * 3 * vertexOffset);
				}

				if (aiMeshPtr->HasTextureCoords(0)) {
					std::vector<float> texCoords;
					for (unsigned int uv_i = 0; uv_i < aiMeshPtr->mNumVertices; uv_i++) {
							texCoords.push_back(aiMeshPtr->mTextureCoords[0][uv_i].x);
							texCoords.push_back(aiMeshPtr->mTextureCoords[0][uv_i].y);
					}
					get_or_emplace<VBO<TexCoord>>(e, nullptr, sizeof(float) * vertexCount * 2)
						.set_data(&texCoords[0], sizeof(float) * aiMeshPtr->mNumVertices * 2, sizeof(float) * vertexOffset * 2);
				}
				
				// TODO: bones
				if (aiMeshPtr->HasBones()) { }

				vertexOffset += aiMeshPtr->mNumVertices;
				faceOffset += aiMeshPtr->mNumFaces;
			}
			
			// TODO: materials???
			// currently only does the first material and doesnt store it

			auto aiMatPtr = scene->mMaterials[scene->mMeshes[node->mMeshes[0]]->mMaterialIndex];
			
			auto& mat = emplace<SpecularMaterial>(e);

			aiColor3D colour;
			aiString texture;

			if (aiMatPtr->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), texture) == AI_SUCCESS)
				mat.set_diff(Texture::get(texture.C_Str()));
			else if (aiMatPtr->Get(AI_MATKEY_COLOR_DIFFUSE, colour) == AI_SUCCESS)
				mat.set_diff(glm::vec3(colour.r, colour.g, colour.b));

			if (aiMatPtr->Get(AI_MATKEY_TEXTURE_SPECULAR(0), texture) == AI_SUCCESS)
				mat.set_spec(Texture::get(texture.C_Str()));
			else if (aiMatPtr->Get(AI_MATKEY_COLOR_SPECULAR, colour) == AI_SUCCESS)
				mat.set_spec(colour.r);

			if (aiMatPtr->Get(AI_MATKEY_TEXTURE_SHININESS(0), texture) == AI_SUCCESS)
				mat.set_glos(Texture::get(texture.C_Str()));
			else if (aiMatPtr->Get(AI_MATKEY_SHININESS, colour) == AI_SUCCESS)
				mat.set_glos(colour.r);
		}

		std::cout << std::endl;

	} while (!sceneStack.empty());

	*/
	
	// removes the first node 
	//destroy(entt_lookup[scene->mRootNode]);
	//entt_lookup.erase(scene->mRootNode);

	// select fist object
	return e; // return scene root 
}



/*

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
*/

/*
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
*/

/*
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


	// if (scene->HasAnimations()) emplace<AnimationHandler>(entt_lookup[scene->mRootNode], scene->mAnimations, scene->mNumAnimations);


	return entt_lookup[scene->mRootNode]; // return scene root
}
*/