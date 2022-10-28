#include "LoadSystem.h"
#include "../Components/Mesh.h"
#include "../Components/Hierarchy.h"
#include "../Components/Transform.h"
#include "../Components/Material.h"
#include "../Rendering/Resources/Texture.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>

/*hierarchy			*DONE*
* transform			*DONE*
* mesh				*DONE*
* material			*DONE*
* animation
* lightEmitter
* shadowCastor
* Bones
* Skinning
* bounding box
*/

/*get optional texture map*/
#define GET_OPTIONAL_MAP(RES, KEY)															\
{																							\
	aiString path;																			\
	if (aiMatPtr->Get(KEY, path) == AI_SUCCESS)												\
	{																						\
		if (path.data[0] == '*')															\
		{																					\
			RES = textures[std::stoi(std::string(path.C_Str()).substr(1, path.length))];	\
		}																					\
		else																				\
		{																					\
			RES = ResourceManager<Texture>::fetch(std::string(path.C_Str())).value();		\
		}																					\
	}																						\
}


/*get variant texture map*/
#define GET_VARIANT_MAP(RES, TYPE, KEY1, KEY2)												\
{																							\
	aiString path;																			\
	TYPE value;																				\
	if (aiMatPtr->Get(KEY1, path) == AI_SUCCESS)											\
	{																						\
		if (path.data[0] == '*')															\
		{																					\
			RES = textures[std::stoi(std::string(path.C_Str()).substr(1, path.length))];	\
		}																					\
		else																				\
		{																					\
			RES = ResourceManager<Texture>::fetch(std::string(path.C_Str())).value();		\
		}																					\
	}																						\
	else if (aiMatPtr->Get(KEY2, value) == AI_SUCCESS)										\
	{																						\
		RES = value;																		\
	}																						\
}

entt::entity LoadSystem::load(std::string filepath)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filepath,
		aiProcess_Triangulate |
		//aiProcess_FlipUVs |
		aiProcess_GlobalScale |
		aiProcess_EmbedTextures |
		//aiProcess_OptimizeGraph | 
		//aiProcess_OptimizeMeshes |
		aiProcess_CalcTangentSpace | 
		aiProcess_GenUVCoords | 
		aiProcess_JoinIdenticalVertices);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "[Loading Error] : " << importer.GetErrorString() << std::endl;
		throw std::exception();
	}

	entt::entity e = create();

	if (scene->HasMaterials()) 
	{
		// array of textures
		std::vector<Texture> textures;
		for (unsigned int i = 0; i < scene->mNumTextures; i++) {
			textures.push_back(ResourceManager<Texture>::fetch_or_default(scene->mTextures[i]->mFilename.C_Str(),
				scene->mTextures[i]->mWidth, 
				scene->mTextures[i]->mHeight,
				Texture::Format::NONE,
				scene->mTextures[i]->pcData,
				Texture::Format::NONE,
				Texture::Wrap::REPEAT,
				Texture::Filter::LINEAR
			));
		}

		std::vector<Material::Data> materials(scene->mNumMaterials);
		for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
			aiMaterial* aiMatPtr = scene->mMaterials[i];

			GET_VARIANT_MAP(materials[i].diffuse, glm::vec3, AI_MATKEY_TEXTURE_DIFFUSE(0), AI_MATKEY_COLOR_DIFFUSE)
			GET_VARIANT_MAP(materials[i].opacity, float, AI_MATKEY_TEXTURE_OPACITY(0), AI_MATKEY_OPACITY)
			GET_VARIANT_MAP(materials[i].specular, glm::vec3, AI_MATKEY_TEXTURE_SPECULAR(0), AI_MATKEY_COLOR_SPECULAR)
			GET_VARIANT_MAP(materials[i].shininess, float, AI_MATKEY_TEXTURE_SHININESS(0), AI_MATKEY_SHININESS)
			GET_VARIANT_MAP(materials[i].emissive, glm::vec3, AI_MATKEY_TEXTURE_EMISSIVE(0), AI_MATKEY_COLOR_EMISSIVE)
			GET_OPTIONAL_MAP(materials[i].ambientOcclusion, AI_MATKEY_TEXTURE_LIGHTMAP(0))
			GET_OPTIONAL_MAP(materials[i].height, AI_MATKEY_TEXTURE_HEIGHT(0))
			GET_OPTIONAL_MAP(materials[i].normal, AI_MATKEY_TEXTURE_NORMALS(0))
		}

		emplace<Material>(e, materials);
	}


	if (scene->HasMeshes())
	{
		// get size needed for buffers
		unsigned int vertexCount = 0, faceCount = 0;
		for (unsigned int mesh_i = 0; mesh_i < scene->mNumMeshes; mesh_i++) {
			vertexCount += scene->mMeshes[mesh_i]->mNumVertices;
			faceCount += scene->mMeshes[mesh_i]->mNumFaces;
		}

		// iteterate over each mesh
		unsigned int vertexOffset = 0, faceOffset = 0, boneOffset = 0;
		for (unsigned int mesh_i = 0; mesh_i < scene->mNumMeshes; mesh_i++)
		{
			auto aiMeshPtr = scene->mMeshes[mesh_i];

			if (aiMeshPtr->HasFaces())
			{
				std::vector<unsigned int> indices(aiMeshPtr->mNumFaces * 3);
				for (unsigned int face_i = 0; face_i < aiMeshPtr->mNumFaces; face_i++)
				{
					auto& face = aiMeshPtr->mFaces[face_i];
					for (unsigned int index_i = 0; index_i < 3; index_i++)
						indices[face_i * 3 + index_i] = face.mIndices[index_i] + vertexOffset;
				}
				get_or_emplace<Mesh::VBO<Mesh::V_Index>>(e, nullptr, sizeof(int) * faceCount * 3)
					.setData(indices.data(), sizeof(int) * 3 * aiMeshPtr->mNumFaces, sizeof(int) * 3 * faceOffset);
			}

			if (aiMeshPtr->HasPositions()) {
				get_or_emplace<Mesh::VBO<Mesh::V_Position>>(e, nullptr, sizeof(float) * 3 * vertexCount)
					.setData(aiMeshPtr->mVertices, sizeof(float) * 3 * aiMeshPtr->mNumVertices, sizeof(float) * 3 * vertexOffset);
			}

			if (aiMeshPtr->HasNormals()) {
				get_or_emplace<Mesh::VBO<Mesh::V_Normal>>(e, nullptr, sizeof(float) * 3 * vertexCount)
					.setData(aiMeshPtr->mNormals, sizeof(float) * 3 * aiMeshPtr->mNumVertices, sizeof(float) * 3 * vertexOffset);

				get_or_emplace<Mesh::VBO<Mesh::V_Tangent>>(e, nullptr, sizeof(float) * 3 * vertexCount)
					.setData(aiMeshPtr->mTangents, sizeof(float) * 3 * aiMeshPtr->mNumVertices, sizeof(float) * 3 * vertexOffset);
			}

			if (aiMeshPtr->HasTextureCoords(0)) {
				std::vector<float> texCoords(aiMeshPtr->mNumVertices * 2);
				for (unsigned int uv_i = 0; uv_i < aiMeshPtr->mNumVertices; uv_i++) {
					texCoords[uv_i * 2 + 0] = aiMeshPtr->mTextureCoords[0][uv_i].x;
					texCoords[uv_i * 2 + 1] = aiMeshPtr->mTextureCoords[0][uv_i].y;
				}
				get_or_emplace<Mesh::VBO<Mesh::V_TexCoord>>(e, nullptr, sizeof(float) * 2 * vertexCount)
					.setData(texCoords.data(), sizeof(float) * 2 * aiMeshPtr->mNumVertices, sizeof(float) * 2 * vertexOffset);
			}

			if (scene->HasMaterials()) {
				unsigned int materialID = get<Material>(e).m_indices[aiMeshPtr->mMaterialIndex].value;
				std::vector<unsigned int> materialIndex(aiMeshPtr->mNumVertices, materialID);
				get_or_emplace<Mesh::VBO<Mesh::V_MaterialIndex>>(e, nullptr, sizeof(int) * 1 * vertexCount)
					.setData(materialIndex.data(), sizeof(int) * 1 * aiMeshPtr->mNumVertices, sizeof(int) * 1 * vertexOffset);
			}

			vertexOffset += aiMeshPtr->mNumVertices;
			faceOffset += aiMeshPtr->mNumFaces;
		}

		std::vector<int> test(vertexOffset);
		get<Mesh::VBO<Mesh::V_MaterialIndex>>(e).getData(test.data(), sizeof(int) * vertexOffset, 0);
	}

	return e;
}