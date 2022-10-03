#include "LoadSystem.h"
#include "../Components/Mesh.h"
#include "../Components/Hierarchy.h"
#include "../Components/Transform.h"
#include "../Components/Material.h"
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
// add bounding box to Transform vao combo for screen space culling optimizations
// boneID and transform indices
// animation

// Framebuffer Needs:
//	position		vec3
//	depth			float
//	normal			vec3
//	texcoord		vec2
//	material Index	unsigned int

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
		//aiProcess_GenUVCoords | 
		aiProcess_JoinIdenticalVertices);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "[Loading Error] : " << importer.GetErrorString() << std::endl;
		throw std::exception();
	}
	
	// array of textures
	std::vector<Texture> textures;
	if (scene->HasTextures()) {
		auto texPtr = scene->mTextures;
		for (unsigned int i = 0; i < scene->mNumTextures; i++) {
			if (scene->mTextures[i])
			textures.push_back(ResourceManager<Texture>::fetch_or_default(texPtr[i]->mFilename.C_Str(),
				texPtr[i]->mWidth, texPtr[i]->mHeight, 
				Texture::Format::RGBA, 
				texPtr[i]->pcData, 
				Texture::Format::NONE, 
				Texture::Wrap::REPEAT,
				Texture::Filter::LINEAR
				));
		}
	}

	entt::entity e = create();

	if (scene->HasMaterials()) {
		std::vector<Material::Data> materials(scene->mNumMaterials);
		
		for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
			aiMaterial* aiMatPtr = scene->mMaterials[i];

			// diffuse map
			{
				aiColor3D colour;
				aiString texture;
				if (aiMatPtr->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), texture) == AI_SUCCESS)
					materials[i].diffuse = ResourceManager<Texture>::fetch(texture.C_Str()).value();
				else if (aiMatPtr->Get(AI_MATKEY_COLOR_DIFFUSE, colour) == AI_SUCCESS)
					materials[i].diffuse = glm::vec3(colour.r, colour.g, colour.b);
			}
			// opacity map
			{
				float value;
				aiString texture;
				if (aiMatPtr->Get(AI_MATKEY_TEXTURE_OPACITY(0), texture) == AI_SUCCESS)
					materials[i].opacity = ResourceManager<Texture>::fetch(texture.C_Str()).value();
				else if (aiMatPtr->Get(AI_MATKEY_OPACITY, value) == AI_SUCCESS)
					materials[i].opacity = value;
			}
			// specular map
			{
				aiColor3D colour;
				aiString texture;
				if (aiMatPtr->Get(AI_MATKEY_TEXTURE_SPECULAR(0), texture) == AI_SUCCESS)
					materials[i].specular = ResourceManager<Texture>::fetch(texture.C_Str()).value();
				else if (aiMatPtr->Get(AI_MATKEY_COLOR_SPECULAR, colour) == AI_SUCCESS)
					materials[i].specular = glm::vec3(colour.r, colour.g, colour.b);

			}
			// shininess map
			{
				float value;
				aiString texture;
				if (aiMatPtr->Get(AI_MATKEY_TEXTURE_SHININESS(0), texture) == AI_SUCCESS)
					materials[i].shininess = ResourceManager<Texture>::fetch(texture.C_Str()).value();
				else if (aiMatPtr->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS)
					materials[i].shininess = value;
			}
			// emissive map
			{
				aiColor3D colour;
				aiString texture;
				if (aiMatPtr->Get(AI_MATKEY_TEXTURE_EMISSIVE(0), texture) == AI_SUCCESS)
					materials[i].shininess = ResourceManager<Texture>::fetch(texture.C_Str()).value();
				else if (aiMatPtr->Get(AI_MATKEY_COLOR_EMISSIVE, colour) == AI_SUCCESS)
					materials[i].emissive = glm::vec3(colour.r, colour.g, colour.b);
			}
			// ambient occlusion map
			{
				aiString texture;
				if (aiMatPtr->Get(AI_MATKEY_TEXTURE_LIGHTMAP(0), texture) == AI_SUCCESS)
					materials[i].ambientOcclusion = ResourceManager<Texture>::fetch(texture.C_Str()).value();
			}
			// height map
			{
				aiString texture;
				if (aiMatPtr->Get(AI_MATKEY_TEXTURE_HEIGHT(0), texture) == AI_SUCCESS)
					materials[i].ambientOcclusion = ResourceManager<Texture>::fetch(texture.C_Str()).value();
			}
			// normal map
			{
				aiString texture;
				if (aiMatPtr->Get(AI_MATKEY_TEXTURE_NORMALS(0), texture) == AI_SUCCESS)
					materials[i].normal = ResourceManager<Texture>::fetch(texture.C_Str()).value();
			}
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
				std::vector<float> texCoords;
				for (unsigned int uv_i = 0; uv_i < aiMeshPtr->mNumVertices; uv_i++) {
					texCoords.push_back(aiMeshPtr->mTextureCoords[0][uv_i].x);
					texCoords.push_back(aiMeshPtr->mTextureCoords[0][uv_i].y);
				}
				get_or_emplace<Mesh::VBO<Mesh::V_TexCoord>>(e, nullptr, sizeof(float) * 2 * vertexCount)
					.setData(texCoords.data(), sizeof(float) * 2 * aiMeshPtr->mNumVertices, sizeof(float) * 2 * vertexOffset);
			}

			/*scene materials*/ {
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