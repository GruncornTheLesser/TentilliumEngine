#include "Mesh.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework

#include <assimp/scene.h>

unsigned int genBuffer(int target, int numOf, const void* data)
{
	unsigned int handle;
	glGenBuffers(1, &handle);
	glBindBuffer(target, handle);
	glBufferData(target, numOf * 4, data, GL_STATIC_DRAW);
	return handle;
}

Mesh::Mesh(const std::shared_ptr<Material> material, 
	const std::vector<unsigned int>& index_data, 
	const std::vector<float>& vertex_data,
	const std::vector<float>* tCoord_data,
	const std::vector<float>* normal_data,
	const std::vector<float>* colour_data)
	: m_mat(material)
{ 
	m_vbo.index = genBuffer(GL_ELEMENT_ARRAY_BUFFER, index_data.size(), &index_data[0]);
	m_vbo.vertex = genBuffer(GL_ARRAY_BUFFER, vertex_data.size(), &vertex_data[0]);
	if (tCoord_data) m_vbo.tCoord = genBuffer(GL_ARRAY_BUFFER, vertex_data.size(), &(*tCoord_data)[0]);
	if (normal_data) m_vbo.normal = genBuffer(GL_ARRAY_BUFFER, vertex_data.size(), &(*normal_data)[0]);
	if (colour_data) m_vbo.colour = genBuffer(GL_ARRAY_BUFFER, vertex_data.size(), &(*colour_data)[0]);

	/*
	// testing
	unsigned int i_test_data[36];
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo.index);
	glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(i_test_data), i_test_data);

	float v_test_data[48];
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo.vertex);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(v_test_data), v_test_data);

	float t_test_data[32];
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo.tCoord);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(t_test_data), t_test_data);
	*/
}


Mesh::Mesh(void* ai_mesh, std::shared_ptr<Material> material)
	: m_mat(material)
{ 
	aiMesh* mesh = static_cast<aiMesh*>(ai_mesh);

	{
		unsigned int* index_data = new unsigned int[mesh->mNumFaces * 3u];
		for (int i = 0; i < mesh->mNumFaces; i++)
		{
			index_data[i * 3] = mesh->mFaces[i].mIndices[0];
			index_data[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
			index_data[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
		}
		m_vbo.index = genBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->mNumFaces * 3, index_data);
	}

	m_vbo.vertex = genBuffer(GL_ARRAY_BUFFER, mesh->mNumVertices * 3, mesh->mVertices);

	// could support n TexCoords
	if (mesh->HasTextureCoords(0))
	{
		float* tCoord_data = new float[mesh->mNumVertices * 2u];
		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			tCoord_data[i * 2] = mesh->mTextureCoords[0][i].x;
			tCoord_data[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
		}
		m_vbo.tCoord = genBuffer(GL_ARRAY_BUFFER, mesh->mNumVertices * 2, tCoord_data);
	}

	if (mesh->HasNormals())
		m_vbo.normal = genBuffer(GL_ARRAY_BUFFER, mesh->mNumVertices * 3, mesh->mNormals);
	
	if (mesh->HasVertexColors(0))
		m_vbo.colour = genBuffer(GL_ARRAY_BUFFER, mesh->mNumVertices * 4, mesh->mColors);
	
	/*
	// testing
	unsigned int* i_test_data = new unsigned int[mesh->mNumFaces * 3];
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo.index);
	glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, mesh->mNumFaces * 3, i_test_data);

	float* v_test_data = new float[mesh->mNumVertices * 3];
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo.vertex);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, mesh->mNumVertices * 3, v_test_data);

	float* t_test_data = new float[mesh->mNumVertices * 2];
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo.tCoord);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, mesh->mNumVertices * 2, t_test_data);
	*/
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &m_vbo.index);
	glDeleteBuffers(1, &m_vbo.vertex);
	glDeleteBuffers(1, &m_vbo.tCoord);
	glDeleteBuffers(1, &m_vbo.normal);
	glDeleteBuffers(1, &m_vbo.colour);
}
