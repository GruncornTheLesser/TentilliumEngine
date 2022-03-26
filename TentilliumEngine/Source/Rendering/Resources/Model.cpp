#include "Model.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework

Model::MeshInstance::MeshInstance(std::shared_ptr<Mesh> mesh, Material material)
	: m_mesh (mesh), m_material(material)
{
	// if (index) -> must have
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->m_vbo.index);

	unsigned int attrib_no = 0;

	// if (position) -> must have
	glBindBuffer(GL_ARRAY_BUFFER, mesh->m_vbo.vertex);
	glVertexAttribPointer(attrib_no, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(attrib_no);
	attrib_no++;

	if (mesh->m_vbo.tCoord)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mesh->m_vbo.tCoord);
		glVertexAttribPointer(attrib_no, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(attrib_no);
		attrib_no++;
	}

	if (mesh->m_vbo.normal)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mesh->m_vbo.normal);
		glVertexAttribPointer(attrib_no, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(attrib_no);
		attrib_no++;
	}

	if (mesh->m_vbo.colour)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mesh->m_vbo.colour);
		glVertexAttribPointer(attrib_no, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(attrib_no);
		attrib_no++;
	}

	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	size /= 4;

	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
}

Model::Model(std::vector<std::shared_ptr<Mesh>> meshes, std::vector<Material> materials)
{
	assert(meshes.size() == materials.size());
	for (int i = 0; i < meshes.size(); i++)
		m_meshes.emplace_back(meshes[i], materials[i]);
}

void Model::draw() const
{
	for (const auto& instance : m_meshes) {
		instance.m_material.bind();
		glBindVertexArray(instance.VAO);
		glDrawElements(GL_TRIANGLES, instance.size, GL_UNSIGNED_INT, NULL);
	}
}
