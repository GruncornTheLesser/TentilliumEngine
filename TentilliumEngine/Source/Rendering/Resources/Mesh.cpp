#include "Mesh.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <assimp/scene.h>


void setupAttrib(int size, unsigned int stride, unsigned int& no, unsigned int& offset)
{
	glVertexAttribPointer(no, size / 4, GL_FLOAT, GL_FALSE, stride, (void*)offset);
	glEnableVertexAttribArray(no);
	offset += size;
	no++;
}

Mesh::Mesh(void* aiPtr, std::shared_ptr<Material> mat) 
{ 
	aiMesh* mesh = static_cast<aiMesh*>(aiPtr);
	
	// add first vertex and setup attributes
	std::vector<int> offsets;
	
	std::vector<float*> vertexdata;
	if (mesh->HasNormals())

	for (int i = 1; i < mesh->mNumVertices; i++)
	{
		mesh->mVertices[i];			//vec3
		mesh->mNormals[i];			// vec3
		mesh->mTextureCoords[i];	// vec2/vec3
		mesh->mBones[i];			// vec4, ivec4
		mesh->mColors[i];			// vec4
		mesh->mTangents[i];			// vec3
		mesh->mBitangents[i];		// vec3
	}
		



}
Mesh::Mesh(std::shared_ptr<Material> material, 
	std::vector<unsigned int>&	index_data,
	std::vector<float>&			vertex_data,
	std::vector<float>*			tCoord_data,
	std::vector<float>*			normal_data,
	std::vector<float>*			colour_data
	//std::vector<glm::ivec4>*	boneID_data,
	//std::vector<glm::vec4>*		boneWT_data
	) : m_mat(material)
{

	// index buffer
	glGenBuffers(1, &m_vbo.index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo.index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data.size() * 4, &index_data[0], GL_STATIC_DRAW);

	// vertex buffer
	glGenBuffers(1, &m_vbo.vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo.vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * 4, &vertex_data[0], GL_STATIC_DRAW);

	// texture coords buffer
	if (tCoord_data)
	{
		glGenBuffers(1, &m_vbo.tCoord_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo.tCoord_buffer);
		glBufferData(GL_ARRAY_BUFFER, tCoord_data->size() * 4, &((*tCoord_data)[0]), GL_STATIC_DRAW);
	}

	// normal buffer
	if (normal_data)
	{
		glGenBuffers(1, &m_vbo.normal_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo.normal_buffer);
		glBufferData(GL_ARRAY_BUFFER, normal_data->size() * 4, &((*normal_data)[0]), GL_STATIC_DRAW);
	}

	if (colour_data)
	{
		glGenBuffers(1, &m_vbo.colour_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo.colour_buffer);
		glBufferData(GL_ARRAY_BUFFER, colour_data->size() * 4, &((*colour_data)[0]), GL_STATIC_DRAW);
	}
	// testing
	unsigned int i_test_data[36];
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo.index_buffer);
	glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(i_test_data), i_test_data);

	float v_test_data[48];
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo.vertex_buffer);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(v_test_data), v_test_data);

	float t_test_data[32];
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo.tCoord_buffer);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(t_test_data), t_test_data);

	// un bind buffers
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
}
/*
Mesh::Mesh(std::vector<float>& vertexdata, std::vector<unsigned int>& indexdata, std::shared_ptr<Material> material, callback_setupVertex vertex)
	: vertexSetup(vertex), mat(material)
{
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	// bind buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// set buffer data
	glBufferData(GL_ARRAY_BUFFER, vertexdata.size() * 4, &(vertexdata[0]), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexdata.size() * 4, &(indexdata[0]), GL_STATIC_DRAW);

	// unbind buffers for safety
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
}
*/