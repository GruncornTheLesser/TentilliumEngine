#include "Model.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework

Model::RenderInfo::~RenderInfo()
{
	glDeleteVertexArrays(1, &VAO);
}

Model::Model(std::vector<std::shared_ptr<Mesh>> meshes)
	: m_meshes(meshes), m_renderInfo(new std::vector<RenderInfo>())
{
	for (auto mesh : meshes) 
	{
		RenderInfo& mesh_instance = m_renderInfo->emplace_back();
		
		glGenVertexArrays(1, &mesh_instance.VAO);
		glBindVertexArray(mesh_instance.VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->index_buffer);

		unsigned int attrib_no = 0;

		glBindBuffer(GL_ARRAY_BUFFER, mesh->vertex_buffer);
		glVertexAttribPointer(attrib_no, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(attrib_no);
		attrib_no++;

		if (mesh->tCoord_buffer)
		{
			glBindBuffer(GL_ARRAY_BUFFER, mesh->tCoord_buffer);
			glVertexAttribPointer(attrib_no, 2, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(attrib_no);
			attrib_no++;
		}

		if (mesh->normal_buffer)
		{
			glBindBuffer(GL_ARRAY_BUFFER, mesh->normal_buffer);
			glVertexAttribPointer(attrib_no, 3, GL_FLOAT, GL_FALSE, 0, NULL); 
			glEnableVertexAttribArray(attrib_no);
			attrib_no++;
		}

		if (mesh->colour_buffer)
		{
			glBindBuffer(GL_ARRAY_BUFFER, mesh->colour_buffer);
			glVertexAttribPointer(attrib_no, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(attrib_no);
			attrib_no++;
		}

		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &mesh_instance.size);
		mesh_instance.size /= 4;

		glBindVertexArray(NULL);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
	}
}

void Model::draw()
{
	for (auto& renderInfo : *m_renderInfo)
	{
		glBindVertexArray(renderInfo.VAO);
		glDrawElements(GL_TRIANGLES, renderInfo.size, GL_UNSIGNED_INT, NULL);
	}
}

