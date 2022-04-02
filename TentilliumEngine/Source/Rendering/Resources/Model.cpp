#include "Model.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework

Model::MeshInstance::MeshInstance(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
	: m_mesh(mesh), m_material(material)
{
	// if (index) -> must have
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->m_buffer_index.handle);

	unsigned int attrib_no = 0;

	// if (position) -> must have
	glBindBuffer(GL_ARRAY_BUFFER, mesh->m_buffer_vertex.handle);
	glVertexAttribPointer(attrib_no, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(attrib_no);
	attrib_no++;

	if (mesh->m_buffer_tCoord.handle)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mesh->m_buffer_tCoord.handle);
		glVertexAttribPointer(attrib_no, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(attrib_no);
		attrib_no++;
	}

	if (mesh->m_buffer_normal.handle)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mesh->m_buffer_normal.handle);
		glVertexAttribPointer(attrib_no, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(attrib_no);
		attrib_no++;
	}

	if (mesh->m_buffer_colour.handle)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mesh->m_buffer_colour.handle);
		glVertexAttribPointer(attrib_no, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(attrib_no);
		attrib_no++;
	}

	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &m_size);
	m_size /= 4;

	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
}

Model::MeshInstance::~MeshInstance()
{
	glDeleteVertexArrays(1, &m_VAO);
}

Model::MeshInstance::MeshInstance(Model::MeshInstance&& other)
{
	this->m_material = other.m_material;
	this->m_mesh = other.m_mesh;
	this->m_size = other.m_size;
	this->m_VAO = other.m_VAO;
}

Model::MeshInstance& Model::MeshInstance::operator=(MeshInstance&& other)
{
	glDeleteVertexArrays(1, &m_VAO);
	
	this->m_material = other.m_material;
	this->m_mesh = other.m_mesh;
	this->m_size = other.m_size;
	this->m_VAO = other.m_VAO;

	return *this;
}

void Model::MeshInstance::draw() const
{
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_size, GL_UNSIGNED_INT, NULL);
}

Model::Model()
{
}

Model::Model(std::vector<std::pair<std::shared_ptr<Mesh>, std::shared_ptr<Material>>> meshes)
{
	for (auto& [mesh, mat] : meshes)
		meshes.emplace_back(mesh, mat);
}

void Model::add(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
{
	m_meshes.emplace_back(mesh, material);
}

/*
Model::Model(const Model&& other)
{
	this->m_meshes = other.m_meshes;
}

Model& Model::operator=(const Model&& other)
{
	this->m_meshes = other.m_meshes;
	return *this;
}
*/
void Model::draw(const ShaderProgram& program) const
{
	
}
