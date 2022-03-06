#include "Mesh.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <assimp/scene.h>

Mesh::Mesh(std::shared_ptr<Material> material,
		size_t				index_size,
		size_t				vertex_size,
		const unsigned int*	index_data,
		const float*		vertex_data,
		const float*		tCoord_data,
		const float*		normal_data,
		const float*		colour_data
	) : m_mat(material)
{
	// index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertex_size, index_data, GL_STATIC_DRAW);

	// vertex buffer
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_size, vertex_data, GL_STATIC_DRAW);

	// texture coords buffer
	if (tCoord_data)
	{
		glGenBuffers(1, &tCoord_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, tCoord_buffer);
		glBufferData(GL_ARRAY_BUFFER, vertex_size, tCoord_data, GL_STATIC_DRAW);
	}

	// normal buffer
	if (normal_data)
	{
		glGenBuffers(1, &normal_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
		glBufferData(GL_ARRAY_BUFFER, vertex_size, normal_data, GL_STATIC_DRAW);
	}

	if (colour_data)
	{
		glGenBuffers(1, &colour_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, colour_buffer);
		glBufferData(GL_ARRAY_BUFFER, vertex_size, colour_data, GL_STATIC_DRAW);
	}

	/*
	// testing
	unsigned int i_test_data[36];
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(i_test_data), i_test_data);

	float v_test_data[48];
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(v_test_data), v_test_data);

	float t_test_data[32];
	glBindBuffer(GL_ARRAY_BUFFER, tCoord_buffer);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(t_test_data), t_test_data);
	*/

	// unbind buffers
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
}
Mesh::~Mesh()
{
	glDeleteBuffers(1, &index_buffer);
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, &tCoord_buffer);
	glDeleteBuffers(1, &normal_buffer);
	glDeleteBuffers(1, &colour_buffer);
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