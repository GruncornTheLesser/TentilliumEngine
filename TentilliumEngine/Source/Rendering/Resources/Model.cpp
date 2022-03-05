#include "Model.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework


Model::RenderInfo::RenderInfo(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
{
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->IBO);

	mesh->vertexSetup();

	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	size /= 4;

	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);

	this->mesh = mesh;
	this->material = material;
}

Model::RenderInfo::~RenderInfo()
{
	glDeleteVertexArrays(1, &VAO);
}

Model::Model(std::vector<std::shared_ptr<Mesh>> meshes, std::vector<std::shared_ptr<Material>> materials)
{
	m_meshes = std::shared_ptr<std::vector<RenderInfo>>(new std::vector<RenderInfo>());

	for (int i = 0; i < std::min(meshes.size(), materials.size()); i++)
		m_meshes->emplace_back(meshes[i], materials[i]);
}

void Model::draw()
{
	for (RenderInfo& renderInfo : *m_meshes)
	{
		glBindVertexArray(renderInfo.VAO);
		glDrawElements(GL_TRIANGLES, renderInfo.size, GL_UNSIGNED_INT, NULL);
	}
}

